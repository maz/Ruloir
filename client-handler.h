#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "prefix.h"
#include "chunk-cache.h"

enum{
	CLIENT_TYPE_REQUEST=0,
	CLIENT_TYPE_FORCE_UPDATE
};

typedef struct ClientNormalRequest{
	int fd;
	char first_char;
} ClientNormalRequest;

typedef struct Client{
	char type;
	union{
		ClientNormalRequest normal_request;
		struct{
			char *key_a;
			char *key_b;
		} force_update;
	} x;
} Client;

typedef struct ClientQueue{
	Client *clients;
	volatile unsigned int idx;
	pthread_mutex_t content_lock;
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

bool ClientHandlerEnqueueClient(ClientHandler *ch,Client *client);

#endif