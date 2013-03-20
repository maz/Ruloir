#include "log.h"
#include "config.h"
#include "utils.h"

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
	GIT_COLOR_BOLD_CYAN "INFO" GIT_COLOR_RESET
};

#define LOG_QUEUE_ENTRY_LIMIT		(100)
#define MAX_STRING_LENGTH			(1024)

enum{
	LOG_QUEUE_ENTRY_BEGIN,
	LOG_QUEUE_ENTRY_END,
	LOG_QUEUE_ENTRY_STRING,
	LOG_QUEUE_ENTRY_NUMBER,
	LOG_QUEUE_ENTRY_OUT_OF_ENTRIES
};

typedef struct LogQueueEntry{
	struct LogQueueEntry *next;
	volatile bool written;	//if false, logging thread doesn't touch
					//if true, client threads can't touch
	volatile char type;
	union{
		struct{
			log_level_t log_level;
			time_t timestamp;
		} commence;
		long number;
		char string[MAX_STRING_LENGTH+1];
	} contents;
} LogQueueEntry;

typedef struct LogQueueList{
	struct LogQueueEntry **head;
	struct LogQueueList *next;
} LogQueueList;

static FILE* log_file=NULL;
static log_level_t log_level_minimum;
static pthread_mutex_t work_to_be_done;
static pthread_key_t my_log_queue_key;
static pthread_mutex_t log_queue_list_lock;
static LogQueueList *log_queue_list=NULL;

static void* logging_thread(void *arg){
	while(1){
		pthread_mutex_lock(&work_to_be_done);
		pthread_mutex_lock(&log_queue_list_lock);
		LogQueueList *lst=log_queue_list;
		do{
			LogQueueEntry *entry=*lst->head;
			char state=0;
			do{
				if(!entry->written)
					break;
				
				entry=entry->next;
			}while(entry!=*lst->head);
			lst=lst->next;
		}while(lst!=log_queue_list);
		log_queue_list=log_queue_list->next;
		pthread_mutex_unlock(&log_queue_list_lock);
	}
	pthread_exit(NULL);
}

static const char* const log_levels_to_strings[]={
	"debug",
	"info",
	"warning",
	"error",
	"fatal"
};

static void parse_log_level_minimum(){
	char *str=strdup(Configuration.log_level_minimum);
	for(char *ptr=str;*ptr;ptr++){
		*ptr=tolower(*ptr);
	}
	for(int i=0;i<LAST_LOG_LEVEL;i++){
		if(strcmp(str,log_levels_to_strings[i])==0){
			log_level_minimum=i;
			return;
		}
	}
	log_level_minimum=LOG_LEVEL_DEBUG;//FALLBACK
	fprintf(stderr, "No log level found '%s'\n",Configuration.log_level_minimum);
}

bool LogOpen(){
	parse_log_level_minimum();
	log_file=fopen(Configuration.log_file_path, "a");
	if(log_file){
		pthread_mutex_init(&work_to_be_done, NULL);
		pthread_mutex_init(&log_queue_list_lock, NULL);
		pthread_key_create(&my_log_queue_key, NULL);
		LogCreateThreadQueue();
		pthread_mutex_trylock(&work_to_be_done);
		return true;
	}else{
		return false;
	}
}
void LogClose(){
	fclose(log_file);
}
void LogCreateThreadQueue(){
	LogQueueEntry *data=malloc(sizeof(LogQueueEntry)*LOG_QUEUE_ENTRY_LIMIT);
	for(unsigned int i=0;i<LOG_QUEUE_ENTRY_LIMIT;i++){
		data[i].written=false;
		data[i].next=((i+1)<LOG_QUEUE_ENTRY_LIMIT)?(&data[i+1]):(&data[0]);
	}
	LogQueueEntry** ptr=malloc(sizeof(LogQueueEntry**));
	*ptr=data;
	pthread_setspecific(my_log_queue_key, ptr);
	pthread_mutex_lock(&log_queue_list_lock);
	if(log_queue_list==NULL){
		log_queue_list=malloc(sizeof(LogQueueList));
		log_queue_list->head=ptr;
		log_queue_list->next=log_queue_list;
	}else{
		LogQueueList *lst=malloc(sizeof(LogQueueList));
		lst->head=ptr;
		lst->next=log_queue_list->next;
		log_queue_list->next=lst;
	}
	pthread_mutex_unlock(&log_queue_list_lock);
}
static LogQueueEntry* next_writable_entry(){
	LogQueueEntry *head=*((LogQueueEntry**)pthread_getspecific(my_log_queue_key));
	LogQueueEntry *entry=head;
	if(entry->written){
		do{
			entry=entry->next;
		}while(entry->written && entry->next!=head);
		if(entry->written){
			entry->type=LOG_QUEUE_ENTRY_OUT_OF_ENTRIES;
			return NULL;
		}
	}
	return entry;
}
#define ADD_ENTRY_HEADER(type_val)	LogQueueEntry *entry=next_writable_entry();	\
								if(entry){	\
									entry->type=type_val;
#define ADD_ENTRY_FOOTER	entry->written=true;	\
		pthread_mutex_unlock(&work_to_be_done);	\
	}
void LogEntryBegin(log_level_t level){
	ADD_ENTRY_HEADER(LOG_QUEUE_ENTRY_BEGIN);
	entry->contents.commence.log_level=level;
	time(&entry->contents.commence.timestamp);
	ADD_ENTRY_FOOTER;
}
void LogEntryPutString(const char *str){
	ADD_ENTRY_HEADER(LOG_QUEUE_ENTRY_STRING);
	size_t len=strlen(str);
	if(len>MAX_STRING_LENGTH){
		len=MAX_STRING_LENGTH;
	}
	memcpy(entry->contents.string,str,len);
	entry->contents.string[len]='\0';
	ADD_ENTRY_FOOTER;
}
void LogEntryPutNumber(long num){
	ADD_ENTRY_HEADER(LOG_QUEUE_ENTRY_NUMBER);
	entry->contents.number=num;
	ADD_ENTRY_FOOTER;
}
void LogEntryEnd(){
	ADD_ENTRY_HEADER(LOG_QUEUE_ENTRY_END);
	ADD_ENTRY_FOOTER;
}