#include "client-handler.h"
#include "utils.h"
#include "config.h"
#include "http-parser.h"
#include "special-request.h"
#include "log.h"

#define BAD_REQUEST		"HTTP/1.0 400 BAD REQUEST\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n400 Bad Request"

ClientHandler *client_handler_head=NULL;

static void* client_handler(void* self_ptr){
	ClientHandler *self=self_ptr;
	HTTPRequest http={0};
	Log(LOG_LEVEL_INFO, LOG_STRING, "Began Client Handler", LOG_END);
	while(1){
		ClientQueue *queue=&self->queues[self->queue_handler_uses];
		
		for(unsigned int i=0;i<queue->idx;i++){
			if(!HandleSpecialClient(self,&queue->clients[i])){
				ClientNormalRequest *client=&queue->clients[i].x.normal_request;
				if(HTTPParse(client->fd,&http))
					//handle_client(self,client,&http);
					self->app->func(&self->cache,client->fd,http.method,http.path,AppChunkGet,AppChunkExists);
				else
					WriteConstStr(client->fd,BAD_REQUEST);
				close(client->fd);
			}
		}
		
		//Switch active queue
		queue->idx=0;
		pthread_mutex_lock(&self->queues[!self->queue_handler_uses].content_lock);
		pthread_mutex_lock(&self->queue_handler_uses_lock);
		self->queue_handler_uses=!self->queue_handler_uses;
		pthread_mutex_unlock(&self->queue_handler_uses_lock);
	}
	pthread_exit(NULL);
	return NULL;
}

bool ClientHandlerEnqueueClient(ClientHandler *ch,Client *client){
	pthread_mutex_lock(&ch->queue_handler_uses_lock);
	bool completed=ClientQueueAdd(&ch->queues[!ch->queue_handler_uses],client);
	pthread_mutex_unlock(&ch->queue_handler_uses_lock);
	return completed;
}

ClientHandler *ClientHandlerNew(){
	ClientHandler *self=malloc(sizeof(ClientHandler));
	self->next=self;
	self->queue_handler_uses=0;
	self->app=NULL;
	ClientQueueInit(&self->queues[0]);
	ClientQueueInit(&self->queues[1]);
	pthread_mutex_init(&self->queue_handler_uses_lock,0);
	pthread_create(&self->thread,0,client_handler,self);
	ChunkCacheInit(&self->cache);
	return self;
}

void ClientHandlerSetApp(ClientHandler *self,App *app){
	if(self->app){
		AppRelease(self->app);
	}
	self->app=AppRetain(app);
}

void ClientQueueInit(ClientQueue* queue){
	queue->idx=0;
	pthread_mutex_init(&queue->content_lock,0);
	pthread_mutex_lock(&queue->content_lock);
	queue->clients=malloc(sizeof(Client)*Configuration.client_queue_length);
}
bool ClientQueueAdd(ClientQueue* queue,Client *client){
	pthread_mutex_unlock(&queue->content_lock);
	if(queue->idx<Configuration.client_queue_length){
		memcpy(&queue->clients[queue->idx++],client,sizeof(Client));
		return true;
	}else{
		return false;
	}
}