#include "client-handler.h"
#include "utils.h"
#include "config.h"

ClientHandler *client_handler_head=NULL;

static void handle_client(ClientHandler *self,Client* client){
	WriteStr(client->fd,"Hello!\n");
	close(client->fd);
}

static void* client_handler(void* self_ptr){
	ClientHandler *self=self_ptr;
	while(1){
		ClientQueue *queue=&self->queues[self->queue_handler_uses];
		
		for(unsigned int i=0;i<queue->idx;i++){
			handle_client(self,&queue->clients[i]);
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