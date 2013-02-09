#include "special-request.h"
#include "utils.h"
#include "client-handler.h"
#include "config.h"

#define COMMAND_LENGTH		(7)

char* read_to_crlf(int fd){
	char ch;
	char *str=malloc(sizeof(char));
	*str=0;
	unsigned int idx=0;
	read(fd,&ch,1);
	while(ch!='\r'){
		str=realloc(str,idx+2);
		str[idx++]=ch;
		str[idx]='\0';
		read(fd,&ch,1);
	}
	read(fd,&ch,1);//read the \n
	return str;
}

void HandleSpecialRequest(struct sockaddr_in *client_addr,int fd){
	size_t len=strlen(Configuration.security_token);
	char *security_token_buffer=malloc(len+1);
	security_token_buffer[len]=0;
	read(fd,security_token_buffer,len);
	if(streq(security_token_buffer,Configuration.security_token)){
		char ch;
		read(fd,&ch,1);//read \r
		read(fd,&ch,1);//read \n
		char command_buffer[COMMAND_LENGTH+2]={0};
		read(fd,command_buffer,COMMAND_LENGTH+2);//plus 2 for \r\n
		command_buffer[COMMAND_LENGTH]=0;
		if(streq(command_buffer,"RELOAD-CACHE")){
			char *a=read_to_crlf(fd);
			char *b=read_to_crlf(fd);
			if(strlen(b)==0){
				free(b);
				b=NULL;
			}
			ClientHandler *handler=client_handler_head;
			do{
				Client client={0};
				client.force_update_key_a=strdup(a);
				client.force_update_key_b=strdup(b);
				if(!ClientHandlerEnqueueClient(handler,&client)){
					free(client.force_update_key_a);
					free(client.force_update_key_b);
				}
				//TODO: what do we do if the client's queue is full (aside from freeing the memory)?
				handler=handler->next;
			}while(handler!=client_handler_head);
			free(a);
			free(b);
		}
	}else{
		WriteStr(fd,"INVALID SECURITY TOKEN");
	}
	close(fd);
}

bool HandleSpecialClient(ClientHandler *handler,Client *client){
	if(client->force_update_key_a){
		ChunkCacheLoadKey(handler->cache,client->force_update_key_a,client->force_update_key_b);
		free(client->force_update_key_a);
		free(client->force_update_key_b);
		return true;
	}else{
		return false;
	}
}