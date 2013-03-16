#include "http-chunk.h"
#include "config.h"
#include "char-buffer.h"
#include "utils.h"
	
static int HTTPFd(){
	SOCKADDR addr;
	size_t size;
	int fd=SocketFromIP(Configuration.http_ip,Configuration.http_port,&size,&addr);
	connect(fd,(struct sockaddr*)&addr,size);
	return fd;
}

static void SendMethod(int fd, const char *method,const char *key_a, const char* key_b){
	write(fd,method,strlen(method));
	WriteConstStr(fd," ");
	write(fd,Configuration.http_path_prefix,strlen(Configuration.http_path_prefix));
	char *enc=urlencode(key_a);
	write(fd,enc,strlen(enc));
	free(enc);
	if(key_b){
		write(fd,Configuration.http_path_delimeter,strlen(Configuration.http_path_delimeter));
		enc=urlencode(key_b);
		write(fd,enc,strlen(enc));
		free(enc);
	}
	write(fd,Configuration.http_path_suffix,strlen(Configuration.http_path_suffix));
	WriteConstStr(fd," HTTP/1.1\r\nConnection: close\r\n\r\n");
}

static bool recieved_200(int fd, CharBuffer *buf){
	char ch;
	do{
		ch=CharBufferRead(fd,buf);
	}while(ch!=' ');
	return ((CharBufferRead(fd,buf)=='2') && (CharBufferRead(fd,buf)=='0') && (CharBufferRead(fd,buf)=='0'));
}

static bool read_until_rn(int fd, CharBuffer *buf){
	bool r_read=false;
	while(buf->len>0){
		char ch=CharBufferRead(fd, buf);
		if(r_read){
			if(ch=='\n')
				return true;
			r_read=false;
		}else if(ch=='\r'){
			r_read=true;
		}
	}
	return false;
}

#define VALUE_OF_DIGIT(ch)	((ch)-'0')

void HTTPChunkGet(void* ctx,Chunk *chunk){
	int fd=HTTPFd();
	SendMethod(fd,"GET",chunk->key_a,chunk->key_b);
	CharBuffer buf=CHAR_BUFFER_INITIALIZER;
	if(recieved_200(fd, &buf)){
		read_until_rn(fd, &buf);
		chunk->len=0;
		char ch=CharBufferRead(fd, &buf);
		//TODO: make this deal with incomplete HTTP responses better
		while(ch!='\r'){
			if(ch=='C' &&
				CharBufferRead(fd, &buf)=='o' &&
				CharBufferRead(fd, &buf)=='n' &&
				CharBufferRead(fd, &buf)=='t' &&
				CharBufferRead(fd, &buf)=='e' &&
				CharBufferRead(fd, &buf)=='n' &&
				CharBufferRead(fd, &buf)=='t' &&
				CharBufferRead(fd, &buf)=='-' &&
				CharBufferRead(fd, &buf)=='L' &&
				CharBufferRead(fd, &buf)=='e' &&
				CharBufferRead(fd, &buf)=='n' &&
				CharBufferRead(fd, &buf)=='g' &&
				CharBufferRead(fd, &buf)=='t' &&
				CharBufferRead(fd, &buf)=='h'
			){
				do{
					ch=CharBufferRead(fd, &buf);
				}while(isspace(ch) || ch==':');
				while(ch!='\r'){
					chunk->len*=10;
					chunk->len+=VALUE_OF_DIGIT(ch);
					ch=CharBufferRead(fd, &buf);
				}
				CharBufferRead(fd, &buf);//read \n
			}else{
				read_until_rn(fd, &buf);
			}
			ch=CharBufferRead(fd, &buf);
		}
		CharBufferRead(fd, &buf);//=> \n
		chunk->value=malloc(chunk->len);
		CharBufferReadMany(fd, &buf, chunk->len, chunk->value);
	}else{
		chunk->len=0;
		chunk->value=NULL;
	}
	close(fd);
}
bool HTTPChunkExists(void* ctx,const char *key){
	int fd=HTTPFd();
	SendMethod(fd,"HEAD",key,NULL);
	CharBuffer buf=CHAR_BUFFER_INITIALIZER;
	bool result=recieved_200(fd, &buf);
	close(fd);
	return result;
}
void* HTTPChunkNewConnection(){return NULL;}
void HTTPChunkCloseConnection(void* x){}
