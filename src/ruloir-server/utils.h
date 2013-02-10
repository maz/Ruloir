#ifndef UTILS_H
#define UTILS_H

#include "prefix.h"

#define SOCKADDR_SIZE	(sizeof(union{struct sockaddr_in6 x;struct sockaddr_in y;}))
typedef char SOCKADDR[SOCKADDR_SIZE];

void ParseAddr(const char *ip,int port,int *af,size_t *size,void* addr);
int SocketFromIP(const char *ip,int port,size_t* size,void* addr);

#define WriteConstStr(fd,str)	write(fd,str,(sizeof(str)/sizeof(char))-1)

char* rulr_strdup(const char *x);
//strdup doesn't use jemalloc (or, more importantly, jemalloc's free)
#undef strdup
#define strdup(x)	rulr_strdup(x)

void memswap(void* a,void* b,size_t sze);
bool streq(const char *a,const char *b);
bool streq_ncs(const char *a,const char *b);
char* urlencode(const char *x);

#endif