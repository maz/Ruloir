#include "special-request.h"
#include "utils.h"
#include "client-handler.h"
#include "config.h"
#include "crc.h"
#include "app.h"

#define COMMAND_LENGTH		(12)

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
				Client client={.type=CLIENT_TYPE_FORCE_UPDATE};
				client.x.force_update.key_a=strdup(a);
				client.x.force_update.key_b=strdup(b);
				if(!ClientHandlerEnqueueClient(handler,&client)){
					free(client.x.force_update.key_a);
					free(client.x.force_update.key_a);
				}
				//TODO: what do we do if the client's queue is full (aside from freeing the memory)?
				handler=handler->next;
			}while(handler!=client_handler_head);
			free(a);
			free(b);
		}else if(streq(command_buffer,"LOAD-PROGRAM")){
			write(fd,Configuration.system_id,strlen(Configuration.system_id));
			WriteConstStr(fd,"\r\n");
			char *str=read_to_crlf(fd);
			int len=atoi(str);
			free(str);
			if(!len){
				close(fd);
				return;
			}
			uint32_t crc_buf=CRC32InitialValue;
			uint32_t crc;
			size_t sze=strlen(Configuration.app_path);
			str=malloc(sze+2);
			memcpy(str,Configuration.app_path,sze);
			str[sze]='~';
			str[sze+1]='\0';
			puts(str);
			FILE *tmp=fopen(str,"wb");
#define CLEANUP_TMP()	({if(tmp){fclose(tmp);}unlink(str);free(str);})
			if(tmp){
				while(len){
					if(read(fd,&ch,1)!=1){
						CLEANUP_TMP();
						close(fd);
						return;
					}
					crc=CRC32(&crc_buf,ch);
					fputc(ch,tmp);
					len--;
				}
				fclose(tmp);
				tmp=NULL;
				read(fd,&crc_buf,4);
				crc=htonl(crc);
				if(crc_buf==crc){
					char *err=NULL;
					App *app=AppOpen(str,&err);
					if(err){
						WriteConstStr(fd,"DLOPEN-ERROR\r\n");
						write(fd,err,strlen(err));
						WriteConstStr(fd,"\r\n");
						free(err);
						CLEANUP_TMP();
					}else{
						ClientHandler *handler=client_handler_head;
						do{
							Client client={.type=CLIENT_TYPE_LOAD_PROGRAM};
							client.x.load_program=app;
							ClientHandlerEnqueueClient(handler,&client);
							//TODO: what do we do if the client's queue is full (aside from freeing the memory)?
							handler=handler->next;
						}while(handler!=client_handler_head);
						WriteConstStr(fd,"OK\r\n\r\n");
						unlink(Configuration.app_path);
						rename(str,Configuration.app_path);
						CLEANUP_TMP();
					}
				}else{
					WriteConstStr(fd,"CRC32-MISMATCH\r\n");
					write(fd,&crc,4);
					WriteConstStr(fd,"\r\n");
					CLEANUP_TMP();
				}
			}else{
				WriteConstStr(fd,"FILE-ERROR\r\n");
				char *buf=calloc(sys_nerr,1);
				strerror_r(errno,buf,sys_nerr);
				write(fd,buf,strlen(buf));
				WriteConstStr(fd,"\r\n");
				free(buf);
			}
		}else{
			WriteConstStr(fd,"COMMAND DOES NOT EXIST");
		}
	}else{
		WriteConstStr(fd,"INVALID SECURITY TOKEN");
	}
	close(fd);
}

bool HandleSpecialClient(ClientHandler *handler,Client *client){
	if(client->type==CLIENT_TYPE_FORCE_UPDATE){
		ChunkCacheLoadKey(handler->cache,client->x.force_update.key_a,client->x.force_update.key_b);
		free(client->x.force_update.key_a);
		free(client->x.force_update.key_b);
		return true;
	}else if(client->type==CLIENT_TYPE_LOAD_PROGRAM){
		ClientHandlerSetApp(handler,client->x.load_program);
		return true;
	}else{
		return false;
	}
}