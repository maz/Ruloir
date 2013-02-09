#include "client-handler.h"
#include "utils.h"
#include "config.h"
#include "http-parser.h"

#define BAD_REQUEST		"HTTP/1.0 400 BAD REQUEST\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n400 Bad Request"

ClientHandler *client_handler_head=NULL;

static void handle_client(ClientHandler *self,Client* client,HTTPRequest *http){
	WriteStr(client->fd,"HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
	Chunk *chunk=ChunkCacheGet(self->cache,http->path+1,NULL);
	write(client->fd,chunk->value,chunk->len);
	close(client->fd);
}

static void* client_handler(void* self_ptr){
	ClientHandler *self=self_ptr;
	HTTPRequest http={0};
	while(1){
		ClientQueue *queue=&self->queues[self->queue_handler_uses];
		
		for(unsigned int i=0;i<queue->idx;i++){
			Client *client=&queue->clients[i];
			if(HTTPParse(client->fd,&http))
				handle_client(self,&queue->clients[i],&http);
			else
				WriteStr(client->fd,BAD_REQUEST);
		}
		
		//Switch active queue
		queue->idx=0;
		pthread_mutex_lock(&self->queue_handler_uses_lock);
		self->queue_handler_uses=!self->queue_handler_uses;
		pthread_mutex_unlock(&self->queue_handler_uses_lock);
	}
	pthread_exit(NULL);
	return NULL;
}

ClientHandler *ClientHandlerNew(){
	ClientHandler *self=malloc(sizeof(ClientHandler));
	self->next=self;
	self->queue_handler_uses=0;
	ClientQueueInit(&self->queues[0]);
	ClientQueueInit(&self->queues[1]);
	pthread_mutex_init(&self->queue_handler_uses_lock,0);
	pthread_create(&self->thread,0,client_handler,self);
	ChunkCacheInit(&self->cache);
	return self;
}

void ClientQueueInit(ClientQueue* queue){
	queue->idx=0;
	queue->clients=malloc(sizeof(Client)*Configuration.client_queue_length);
}
bool ClientQueueAdd(ClientQueue* queue,Client *client){
	if(queue->idx<Configuration.client_queue_length){
		memcpy(&queue->clients[queue->idx++],client,sizeof(Client));
		return true;
	}else{
		return false;
	}
}