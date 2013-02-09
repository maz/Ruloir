#ifndef SPECIAL_REQUEST_H
#define SPECIAL_REQUEST_H

#include "prefix.h"
#include "client-handler.h"

/*
PROTOCOL: all commands are seven characters
	\5
	{security-token}, followed by <CR><LF>
	{command: RELOAD-CACHE or LOAD-PROGRAM}, followed by <CR><LF>
	if RELOAD-CACHE, key a, followed by <CR><LF>, key b, followed by <CR><LF>
		(an empty string will be assumed as no key b)
*/

void HandleSpecialRequest(struct sockaddr_in *client_addr,int fd);

//Returns true if the client was special and has been handled
bool HandleSpecialClient(ClientHandler *handler,Client *client);

#endif