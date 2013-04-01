#include "prefix.h"
#include "config.h"
#include "utils.h"
#include "client-handler.h"
#include "special-request.h"
#include "chunk.h"
#include "log.h"

//Help me Obi-Wan Kenobi...
#define FAIL_NO_LOG(x)			({perror(#x);exit(errno);})
#define ONLY_HOPE(x)			if((x)){FAIL_NO_LOG(x);}
#define ONLY_HOPE_GTE0(x)		ONLY_HOPE((x)<0)

#define CLIENT_QUEUE_LENGTH			(20)
#define MAX_CHUNCK_CACHE_LENGTH		(20)

static bool running=true;
static int serverfd;

static void stop_running(int sig){
	running=false;
	puts("Shutting down...");
	close(serverfd);
}

int main(int argc, char **argv){
	signal(SIGINT,stop_running);
	signal(SIGTERM,stop_running);
	
	if(argc<2){
		fprintf(stderr,"Not enough arguments\n");
		exit(1);
	}
	
	long num_cpus=sysconf(_SC_NPROCESSORS_ONLN);
	if(num_cpus>0){
		Configuration.default_handler_threads=num_cpus;
	}
	
	ConfigurationLoad(argv[1]);
	if(!LogOpen())
		FAIL_NO_LOG();
	
	ONLY_HOPE(!ChunkBackendLoad());
	
	char *err;
	App *app=AppOpen(Configuration.app_path,&err);
	if(!app){
		fprintf(stderr,"Unable to open app at path '%s'\n\t%s\n",Configuration.app_path,err);
		exit(1);
	}
	
	size_t size;
	SOCKADDR addr;
	ONLY_HOPE_GTE0(serverfd=SocketFromIP(Configuration.bind,Configuration.port,&size,addr));
	
	ONLY_HOPE(bind(serverfd,(struct sockaddr*)addr,size));
	ONLY_HOPE(listen(serverfd,Configuration.max_waiting_clients));
	
	Log(LOG_LEVEL_INFO,
		LOG_STRING, "Server running on ",
		LOG_STRING, Configuration.bind,
		LOG_STRING, ":",
		LOG_NUMBER, (long)Configuration.port,
		LOG_END
	);
	
	client_handler_head=ClientHandlerNew();
	ClientHandler *handler=client_handler_head;
	ClientHandlerSetApp(handler,app);
	for(int i=1;i<Configuration.default_handler_threads;i++){
		ClientHandler *handle=ClientHandlerNew();
		ClientHandlerSetApp(handle, app);
		handle->next=handler->next;
		handler->next=handle;
	}
	while(running){
		int fd;
		struct sockaddr_in client_addr;
		socklen_t addrlen=sizeof(client_addr);
		fd=accept(serverfd,(struct sockaddr*)&client_addr,&addrlen);
		if(fd<0){
			continue;
		}
		
		char buf;
		read(fd,&buf,1);
		if(buf=='\5'){
			//This is a special request
			HandleSpecialRequest(&client_addr,fd);
			continue;
		}
		Client client={.type=CLIENT_TYPE_REQUEST,0};
		client.x.normal_request.fd=fd;
		client.x.normal_request.first_char=buf;
		
		ClientHandler *ch=handler;
		bool completed=false;
		do{
			completed=ClientHandlerEnqueueClient(ch,&client);
			ch=handler->next;
		}while(!completed && ch!=handler);
		if(!completed){
			ClientHandler *buf=handler->next;
			ch=ClientHandlerNew();
			ClientHandlerSetApp(ch,app);
			ch->next=buf;
			handler->next=ch;
			completed=ClientHandlerEnqueueClient(ch,&client);
		}
		handler=ch;
	}
	handler=client_handler_head;
	do{
		Client client={.type=CLIENT_TYPE_LOAD_PROGRAM};
		client.x.load_program=NULL;
		ClientHandlerEnqueueClient(handler,&client);
		void *retval;
		pthread_join(handler->thread, &retval);
		handler=handler->next;
	}while(handler!=client_handler_head);
	LogClose();
	exit(0);
}