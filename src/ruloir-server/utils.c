#include "utils.h"
#include "prefix.h"

//The following two functions are from: http://www.geekhideout.com/urlcode.shtml
static inline char to_hex(char code) {
  static const char hex[] = "0123456789abcdef";
  return hex[code & 15];
}
char *urlencode(const char *pstr) {
	char *buf = malloc(strlen(pstr) * 3 + 1), *pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
			*pbuf++ = *pstr;
		else if (*pstr == ' ') 
			*pbuf++ = '+';
		else 
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

bool streq(const char *a,const char *b){
	if(!a && !b)
		return true;
	if(!a || !b)
		return false;
	while(*a && *b && *a==*b){
		++a;
		++b;
	}
	return *a==*b;
}

bool streq_ncs(const char *a,const char *b){
	if(!a && !b)
		return true;
	if(!a || !b)
		return false;
	while(*a && *b && tolower(*a)==tolower(*b)){
		++a;
		++b;
	}
	return *a==*b;
}

void memswap(void* a,void* b,size_t sze){
	unsigned char *x=a;
	unsigned char *y=b;
	while(sze){
		unsigned char tmp=*x;
		*x=*y;
		*y=tmp;
		++x;
		++y;
		sze--;
	}
}

char* rulr_strdup(const char *x){
	if(!x)
		return NULL;
	size_t len=strlen(x)+1;
	char *copy=malloc(len);
	memcpy(copy,x,len);
	return copy;
}

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
		fprintf(stderr,"%s ; %d\n",ip,port);
		perror("Unable to parse ip");
		abort();
	}
}

int SocketFromIP(const char *ip,int port,size_t *size,void* addr){
	int af;
	ParseAddr(ip,port,&af,size,addr);
	return socket(af,SOCK_STREAM,0);
}