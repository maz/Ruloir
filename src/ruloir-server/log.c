#include "log.h"
#include "config.h"
#include "utils.h"
#include "atomics.h"

//The following macros are from the source of Git

#define GIT_COLOR_NORMAL          ""
#define GIT_COLOR_RESET           "\033[m"
#define GIT_COLOR_BOLD            "\033[1m"
#define GIT_COLOR_RED             "\033[31m"
#define GIT_COLOR_GREEN           "\033[32m"
#define GIT_COLOR_YELLOW          "\033[33m"
#define GIT_COLOR_BLUE            "\033[34m"
#define GIT_COLOR_MAGENTA         "\033[35m"
#define GIT_COLOR_CYAN            "\033[36m"
#define GIT_COLOR_BOLD_RED        "\033[1;31m"
#define GIT_COLOR_BOLD_GREEN      "\033[1;32m"
#define GIT_COLOR_BOLD_YELLOW     "\033[1;33m"
#define GIT_COLOR_BOLD_BLUE       "\033[1;34m"
#define GIT_COLOR_BOLD_MAGENTA    "\033[1;35m"
#define GIT_COLOR_BOLD_CYAN       "\033[1;36m"
#define GIT_COLOR_BG_RED          "\033[41m"
#define GIT_COLOR_BG_GREEN        "\033[42m"
#define GIT_COLOR_BG_YELLOW       "\033[43m"
#define GIT_COLOR_BG_BLUE         "\033[44m"
#define GIT_COLOR_BG_MAGENTA      "\033[45m"
#define GIT_COLOR_BG_CYAN         "\033[46m"

static const char* const colored_log_levels[]={
	GIT_COLOR_BOLD_MAGENTA "DEBUG" GIT_COLOR_RESET,
	GIT_COLOR_BOLD_CYAN "INFO" GIT_COLOR_RESET,
	GIT_COLOR_BOLD_YELLOW "WARNING" GIT_COLOR_RESET,
	GIT_COLOR_BOLD_RED "ERROR" GIT_COLOR_RESET,
	GIT_COLOR_BOLD_BLUE "FATAL" GIT_COLOR_RESET
};

static const char* const log_levels_to_strings[]={
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"FATAL"
};

#define MAX_STRING_LENGTH			(1024)

enum{
	LOG_QUEUE_ENTRY_BEGIN,
	LOG_QUEUE_ENTRY_STRING,
	LOG_QUEUE_ENTRY_NUMBER,
	LOG_QUEUE_ENTRY_HEX_REPR
};

typedef struct LogQueueCommand{
	char type;
	union{
		struct{
			log_level_t log_level;
			time_t timestamp;
			pthread_t thread;
			#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INCLUDE_LOCATION
				const char *file;
				int line;
				const char *function;
			#endif
		} commence;
		long number;
		char string[MAX_STRING_LENGTH+1];
	} contents;
} LogQueueCommand;

#define LOG_QUEUE_ENTRY_LENGTH		(10)

typedef struct LogQueueEntry{
	struct LogQueueEntry *next;
	volatile bool written;
	volatile bool reserved;
	volatile unsigned int command_length;
	LogQueueCommand commands[LOG_QUEUE_ENTRY_LENGTH];
} LogQueueEntry;

static LogQueueEntry *log_queue;
static FILE* log_file=NULL;
static pthread_mutex_t log_queue_work_to_be_done;
static bool out_of_entries=false;
log_level_t LogLevelMinimum;
//We don't want logging to necessitate a syscall
static time_t current_time;

static void logging_write(const char *format, ...){
	va_list args;
	va_start(args, format);
	vfprintf(log_file, format, args);
	va_end(args);
	if(Configuration.log_file_stderr_mirror){
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
	}
}

static void logging_write_hex_repr(const void* ptr, size_t len){
	const char *bytes=ptr;
	logging_write("0x");
	for(unsigned int i=0;i<len;i++){
		logging_write("%x", (unsigned)bytes[i]);
	}
}

#define MAX_DATE_LENGTH		(100)
#define DATE_FORMAT			"%Y-%m-%d %H:%M:%S"
	
static void log_queue_command_write(LogQueueCommand *entry){
	const char* const *arr=(Configuration.log_file_color?colored_log_levels:log_levels_to_strings);
	if(entry->type==LOG_QUEUE_ENTRY_BEGIN){
		struct tm cal;
		gmtime_r(&entry->contents.commence.timestamp, &cal);
		char timestamp[MAX_DATE_LENGTH+1]={0};
		if(!strftime(timestamp, MAX_DATE_LENGTH, DATE_FORMAT, &cal))
			timestamp[0]='\0';
		logging_write("\n%s (%s, ", arr[entry->contents.commence.log_level], timestamp);
		logging_write_hex_repr(entry->contents.commence.thread, sizeof(pthread_t));
		logging_write("): ");
	}else if(entry->type==LOG_QUEUE_ENTRY_STRING){
		logging_write("%s", entry->contents.string);
	}else if(entry->type==LOG_QUEUE_ENTRY_NUMBER){
		logging_write("%ld", entry->contents.number);
	}else if(entry->type==LOG_QUEUE_ENTRY_HEX_REPR){
		size_t len;
		memcpy(&len,entry->contents.string,sizeof(size_t));
		logging_write_hex_repr(entry->contents.string+sizeof(size_t), len);
	}
}

static LogQueueCommand out_of_entries_commands[]={
	{
		LOG_QUEUE_ENTRY_BEGIN,
		{
			.commence={
				.log_level=LOG_LEVEL_WARNING,
				.timestamp=0,
				.thread=0,
				#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INCLUDE_LOCATION
				.file=__FILE__,
				.line=135,
				.function="<none>"
				#endif
			}
		}
	},
	{
		LOG_QUEUE_ENTRY_STRING,
		{
			.string="Out of entries!"
		}
	}
};

static void* logging_thread(void *arg){
	logging_write("\nLog opened");
	while(1){
		pthread_mutex_lock(&log_queue_work_to_be_done);
		if(out_of_entries){
			time(&out_of_entries_commands[0].contents.commence.timestamp);
			for(unsigned int i=0;i<(sizeof(out_of_entries_commands)/sizeof(LogQueueCommand));i++){
				log_queue_command_write(&out_of_entries_commands[i]);
			}
		}
		while(log_queue->written){
			for(unsigned char i=0;i<log_queue->command_length;i++){
				log_queue_command_write(&log_queue->commands[i]);
			}
			log_queue->reserved=false;
			log_queue->written=false;
			log_queue=log_queue->next;
			out_of_entries=false;//Do this here, since we'll have at least one more entry
		}
		
		if(Configuration.log_file_stderr_mirror)
			fflush(stderr);
		fflush(log_file);
	}
	pthread_exit(NULL);
}

static void parse_LogLevelMinimum(){
	char *str=strdup(Configuration.log_level_minimum);
	for(char *ptr=str;*ptr;ptr++){
		*ptr=toupper(*ptr);
	}
	for(int i=0;i<LAST_LOG_LEVEL;i++){
		if(strcmp(str,log_levels_to_strings[i])==0){
			LogLevelMinimum=i;
			return;
		}
	}
	LogLevelMinimum=LOG_LEVEL_DEBUG;//FALLBACK
	fprintf(stderr, "No log level found '%s'\n",Configuration.log_level_minimum);
}

static void* update_time_thread(void* arg){
	while(1){
		sleep(1);
		time(&current_time);
	}
	pthread_exit(NULL);
}

bool LogOpen(){
	parse_LogLevelMinimum();
	log_file=fopen(Configuration.log_file_path, "a");
	if(log_file){
		log_queue=malloc(sizeof(LogQueueEntry)*Configuration.log_queue_length);
		for(unsigned int i=0;i<Configuration.log_queue_length;i++){
			log_queue[i].written=false;
			log_queue[i].reserved=false;
			log_queue[i].next=((i+1)<Configuration.log_queue_length)?&log_queue[i+1]:&log_queue[0];
		}
		pthread_mutex_init(&log_queue_work_to_be_done, NULL);
		pthread_mutex_trylock(&log_queue_work_to_be_done);
		time(&current_time);
		pthread_t tid;
		pthread_create(&tid, NULL, logging_thread, NULL);
		pthread_create(&tid, NULL, update_time_thread, NULL);
		return true;
	}else{
		return false;
	}
}
void LogClose(){
	pthread_mutex_destroy(&log_queue_work_to_be_done);
	fclose(log_file);
}

#define TOO_MANY_CMDS_MSG		"Too many commands were attempted to be written to the log buffer."

void Log_Internal(
#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INCLUDE_LOCATION
	const char *file,
	int line,
	const char *function,
#endif
	log_level_t log_level, ...
){
	if(log_level<LogLevelMinimum || out_of_entries)//If we're out of entries, then there's no point visiting to make sure
		return;
	bool failure=true;
	LogQueueEntry *entry=log_queue;
	do{
		//CompareAndSwap returns the old value, which we want to be false
		if(CompareAndSwap(&entry->reserved, false, true)==false){
			failure=false;
			break;
		}
		entry=entry->next;
	}while(entry!=log_queue);
	if(failure){
		out_of_entries=true;
	}else{
		entry->command_length=1;
		LogQueueCommand *cmd=entry->commands;
		cmd->type=LOG_QUEUE_ENTRY_BEGIN;
		cmd->contents.commence.log_level=log_level;
		cmd->contents.commence.timestamp=current_time;
		cmd->contents.commence.thread=pthread_self();
		#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INCLUDE_LOCATION
		cmd->contents.commence.line=line;
		cmd->contents.commence.file=file;
		cmd->contents.commence.function=function;
		#endif
		va_list lst;
		va_start(lst, log_level);
		int type;
#define INCR()		({++entry->command_length;cmd++;})
		INCR();
		while((type=va_arg(lst, int))!=LOG_END && entry->command_length<=LOG_QUEUE_ENTRY_LENGTH){
			if(type==LOG_NUMBER){
				long num=va_arg(lst, long);
				cmd->type=LOG_QUEUE_ENTRY_NUMBER;
				cmd->contents.number=num;
			}else if(type==LOG_STRING){
				const char *str=va_arg(lst, const char*);
				cmd->type=LOG_QUEUE_ENTRY_STRING;
				int len=strlen(str);
				if(len>MAX_STRING_LENGTH){
					Log_Internal(
						#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INCLUDE_LOCATION
							file, line, function,
						#endif
						LOG_LEVEL_WARNING,
						LOG_STRING, "Attemped to log string of length",
						LOG_NUMBER, (long) len,
						LOG_STRING, "which was longer than the max, set at compile-time to",
						LOG_NUMBER, (long) MAX_STRING_LENGTH,
						LOG_END
					);
					len=MAX_STRING_LENGTH;
				}
				memcpy(cmd->contents.string, str, len);
				cmd->contents.string[len]='\0';
			}
			INCR();
		}
#undef INCR
		va_end(lst);
		if(entry->command_length>LOG_QUEUE_ENTRY_LENGTH){
			entry->command_length=2;
			entry->commands[0].contents.commence.log_level=LOG_LEVEL_ERROR;
			entry->commands[1].type=LOG_QUEUE_ENTRY_STRING;
			memcpy(TOO_MANY_CMDS_MSG,entry->commands[1].contents.string,strlen(TOO_MANY_CMDS_MSG)+1);
		}
		entry->written=true;
		//If we're out of entries, we'd better be processing them, so we don't need to unlock
		//Moreover, if we've run out of log entries, we're probably under high load, so let's be SLIGHTLY more performance efficient
		pthread_mutex_unlock(&log_queue_work_to_be_done);
	}
}