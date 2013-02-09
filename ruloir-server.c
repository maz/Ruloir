#include "prefix.h"
#include "config.h"
#include "utils.h"
#include "client-handler.h"
#include "special-request.h"

//Help me Obi-Wan Kenobi...
#define ONLY_HOPE(x)		if((x)){perror(#x);exit(errno);}
#define ONLY_HOPE_GTE0(x)	ONLY_HOPE((x)<0)

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
	ConfigurationLoad(argv[1]);
	
	size_t size;
	SOCKADDR addr;
	ONLY_HOPE_GTE0(serverfd=SocketFromIP(Configuration.bind,Configuration.port,&size,addr));
	
	ONLY_HOPE(bind(serverfd,(struct sockaddr*)addr,size));
	ONLY_HOPE(listen(serverfd,Configuration.max_waiting_clients));
	
	client_handler_head=ClientHandlerNew();
	ClientHandler *handler=client_handler_head;
	while(running){
		int fd;
		struct sockaddr_in client_addr;
		socklen_t addrlen=sizeof(client_addr);
		fd=accept(serverfd,(struct sockaddr*)&client_addr,&addrlen);
		
		char buf;
		read(fd,&buf,1);
		if(buf=='\5'){
			//This is a special request
			HandleSpecialRequest(&client_addr,fd);
			continue;
		}
		Client client={.fd=fd,.first_char=buf,0};
		
		ClientHandler *ch=handler;
		bool completed=false;
		do{
			completed=ClientHandlerEnqueueClient(ch,&client);
			ch=handler->next;
		}while(!completed && ch!=handler);
		if(!completed){
			ClientHandler *buf=handler->next;
			ch=ClientHandlerNew();
			ch->next=buf;
			handler->next=ch;
			completed=ClientHandlerEnqueueClient(ch,&client);
		}
		handler=ch;
	}
	exit(0);
}