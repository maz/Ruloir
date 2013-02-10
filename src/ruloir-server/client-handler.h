#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "prefix.h"
#include "chunk-cache.h"
#include "app.h"

enum{
	CLIENT_TYPE_REQUEST=0,
	CLIENT_TYPE_FORCE_UPDATE,
	CLIENT_TYPE_LOAD_PROGRAM
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
		App* load_program;
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
	App *app;
} ClientHandler;

extern ClientHandler *client_handler_head;

ClientHandler *ClientHandlerNew();
bool ClientHandlerEnqueueClient(ClientHandler *ch,Client *client);
void ClientHandlerSetApp(ClientHandler *self,App *app);

void ClientQueueInit(ClientQueue* queue);
bool ClientQueueAdd(ClientQueue* queue,Client *client);

#endif