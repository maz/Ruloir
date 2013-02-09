#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "prefix.h"
#include "chunk-cache.h"

typedef struct Client{
	int fd;
	char first_char;
	char *force_update_key;
} Client;

typedef struct ClientQueue{
	Client *clients;
	volatile unsigned int idx;
} ClientQueue;

typedef struct ClientHandler{
	pthread_t thread;
	struct ClientHandler *next;
	ClientQueue queues[2];
	volatile char queue_handler_uses;
	pthread_mutex_t queue_handler_uses_lock;
	
	ChunkCache cache;
} ClientHandler;

extern ClientHandler *client_handler_head;

ClientHandler *ClientHandlerNew();

void ClientQueueInit(ClientQueue* queue);
bool ClientQueueAdd(ClientQueue* queue,Client *client);

#endif