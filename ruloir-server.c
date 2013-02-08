#include "prefix.h"
#include "chunk.h"
#include "config.h"
#include "utils.h"

//Help me Obi-Wan Kenobi...
#define ONLY_HOPE(x)		if((x)){perror(#x);exit(errno);}
#define ONLY_HOPE_SET(x)	ONLY_HOPE((x)<0)

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
	int af;
	size_t size;
	char addr[SOCKADDR_SIZE]={0};
	ParseAddr(Configuration.bind,Configuration.port,&af,&size,addr);
	ONLY_HOPE_SET(serverfd=socket(af,SOCK_STREAM,0));
	
	ONLY_HOPE(bind(serverfd,(struct sockaddr*)addr,size));
	ONLY_HOPE(listen(serverfd,Configuration.max_waiting_clients));
	while(running){
		int fd;
		struct sockaddr_in client_addr;
		socklen_t addrlen=sizeof(client_addr);
		fd=accept(serverfd,(struct sockaddr*)&client_addr,&addrlen);
		
		char buf[100];
		write(fd,buf,read(fd,buf,100));
		close(fd);
	}
	return 0;
}