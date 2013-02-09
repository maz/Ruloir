#include "prefix.h"
#include "chunk.h"
#include "utils.h"
#include "config.h"

#define LENGTH_BUFFER_LENGTH	(7)
#define WriteStr(fd,str)	write(fd,str,(sizeof(str)/sizeof(char))-1)

static void write_param(int fd,const char *str){
	WriteStr(fd,"$");
	char buf[LENGTH_BUFFER_LENGTH]={0};//Longest Redis Key size is 512K
	int len=strlen(str);
	snprintf(buf,LENGTH_BUFFER_LENGTH,"%d",len);
	write(fd,buf,strlen(buf));
	WriteStr(fd,"\r\n");
	write(fd,str,strlen(str));
	WriteStr(fd,"\r\n");
}

void ChunkGet(Chunk *chunk){
	SOCKADDR addr;
	size_t size;
	int fd=SocketFromIP(Configuration.redis_ip,Configuration.redis_port,&size,&addr);
	connect(fd,(struct sockaddr*)&addr,size);
	if(chunk->key_b){
		WriteStr(fd,"*3\r\n$4\r\nHGET\r\n");
		write_param(fd,chunk->key_a);
		write_param(fd,chunk->key_b);
	}else{
		WriteStr(fd,"*2\r\n$3\r\nGET\r\n");
		write_param(fd,chunk->key_a);
	}
	char ch;
	read(fd,&ch,sizeof(char));
	char buf[LENGTH_BUFFER_LENGTH]={0};
	char i=0;
	read(fd,&ch,sizeof(char));
	while(i<LENGTH_BUFFER_LENGTH && ch!='\r'){
		buf[i]=ch;
		read(fd,&ch,sizeof(char));
		++i;
	}
	//assume we've hit the \r
	read(fd,&ch,sizeof(char));//read the \n
	chunk->len=atoi(buf);
	chunk->value=malloc(chunk->len);
	if(read(fd,chunk->value,chunk->len)!=chunk->len){
		fprintf(stderr,"Unable to read needed bytes of chunk\n");
		abort();
	}
	read(fd,buf,2);//read the \r\n
	close(fd);
}

void ChunkFreeContents(Chunk *chunk){
	free(chunk->key_a);
	free(chunk->key_b);
	free(chunk->value);
	memset(chunk,0,sizeof(Chunk));
}
