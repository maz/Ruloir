#ifndef UTILS_H
#define UTILS_H

#include "prefix.h"

#define SOCKADDR_SIZE	(sizeof(union{struct sockaddr_in6 x;struct sockaddr_in y;}))
void ParseAddr(const char *ip,int port,int *af,size_t *size,void* addr);

#endif