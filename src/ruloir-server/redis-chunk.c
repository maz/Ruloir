#include "prefix.h"
#include "redis-chunk.h"
#include "utils.h"
#include "config.h"
#include "char-buffer.h"

#define LENGTH_BUFFER_LENGTH	(7)

static void write_param(int fd,const char *str){
	WriteConstStr(fd,"$");
	char buf[LENGTH_BUFFER_LENGTH]={0};//Longest Redis Key size is 512K
	int len=strlen(str);
	snprintf(buf,LENGTH_BUFFER_LENGTH,"%d",len);
	write(fd,buf,strlen(buf));
	WriteConstStr(fd,"\r\n");
	write(fd,str,strlen(str));
	WriteConstStr(fd,"\r\n");
}

static int RedisFd(){
	SOCKADDR addr;
	size_t size;
	int fd=SocketFromIP(Configuration.redis_ip,Configuration.redis_port,&size,&addr);
	connect(fd,(struct sockaddr*)&addr,size);
	return fd;
}

void RedisChunkGet(void* conn,Chunk *chunk){
	int fd=RedisFd();
	if(chunk->key_b){
		WriteConstStr(fd,"*3\r\n$4\r\nHGET\r\n");
		write_param(fd,chunk->key_a);
		write_param(fd,chunk->key_b);
	}else{
		WriteConstStr(fd,"*2\r\n$3\r\nGET\r\n");
		write_param(fd,chunk->key_a);
	}
	CharBuffer *char_buf=CharBufferNew();
	char ch;
	ch=CharBufferRead(fd,char_buf);
	char buf[LENGTH_BUFFER_LENGTH]={0};
	char i=0;
	ch=CharBufferRead(fd,char_buf);
	while(i<LENGTH_BUFFER_LENGTH && ch!='\r'){
		buf[i]=ch;
		ch=CharBufferRead(fd,char_buf);
		++i;
	}
	//assume we've hit the \r
	ch=CharBufferRead(fd,char_buf);//read the \n
	chunk->len=atoi(buf);
	if(chunk->len>0){
		//FOUND
		chunk->value=malloc(chunk->len);
		CharBufferReadMany(fd, char_buf, chunk->len, chunk->value);
		CharBufferReadMany(fd, char_buf, 2, buf);//read the \r\n
	}else{
		//NOT FOUND
		chunk->value=NULL;
	}
	CharBufferFree(char_buf);
	close(fd);
}

bool RedisChunkExists(void* conn,const char *key){
	int fd=RedisFd();
	WriteConstStr(fd,"*2\r\n$6\r\nEXISTS\r\n");
	write_param(fd,key);
	WriteConstStr(fd,"\r\n");
	char buf[LENGTH_BUFFER_LENGTH+2];//+1 colon, +1 \0
	read(fd,buf,LENGTH_BUFFER_LENGTH+2);
	close(fd);
	return atoi(buf+1)!=0;
}


void* RedisChunkNewConnection(){
	return NULL;
}
void RedisChunkCloseConnection(void* conn){
	
}