#include "utils.h"
#include "prefix.h"

void ParseAddr(const char *ip,int port,int *af,size_t *size,void* addr){
	struct in6_addr in6;
	struct in_addr in4;
	bzero(addr,SOCKADDR_SIZE);
	if(inet_pton(AF_INET,ip,&in4)==1){
		*af=AF_INET;
		*size=sizeof(struct sockaddr_in);
		((struct sockaddr_in*)addr)->sin_family=AF_INET;
		((struct sockaddr_in*)addr)->sin_port=htons(port);
		((struct sockaddr_in*)addr)->sin_addr=in4;
	}else if(inet_pton(AF_INET6,ip,&in6)==1){
		*af=AF_INET6;
		*size=sizeof(struct sockaddr_in6);
		((struct sockaddr_in6*)addr)->sin6_family=AF_INET6;
		((struct sockaddr_in6*)addr)->sin6_port=htons(port);
		((struct sockaddr_in6*)addr)->sin6_addr=in6;
	}else{
		perror("Unable to parse ip");
		abort();
	}
}

int SocketFromIP(const char *ip,int port,size_t *size,void* addr){
	int af;
	ParseAddr(ip,port,&af,size,addr);
	return socket(af,SOCK_STREAM,0);
}