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
	WriteConstStr(fd," http/1.1\r\nConnection: close\r\n\r\n");
}

void HTTPChunkGet(void* ctx,Chunk *chunk){
	int fd=HTTPFd();
	SendMethod(fd,"GET",chunk->key_a,chunk->key_b);
	CharBuffer buf=CHAR_BUFFER_INITIALIZER;
	char okay=0;
	while(okay!=4){
		char ch=CharBufferRead(fd,&buf);
		switch(okay){
		case 0:
			okay=(ch=='\r')?1:0;
			break;
		case 2:
			okay=(ch=='\r')?2:0;
			break;
		case 1:
			okay=(ch=='\n')?3:0;
			break;
		case 3:
			okay=(ch=='\n')?4:0;
			break;
		}
	}
	
	close(fd);
}
bool HTTPChunkExists(void* ctx,const char *key){
	int fd=HTTPFd();
	SendMethod(fd,"HEAD",key,NULL);
	CharBuffer buf=CHAR_BUFFER_INITIALIZER;
	char ch;
	do{
		CharBufferRead(fd,&buf);
	}while(ch!=' ');
	bool result=((CharBufferRead(fd,&buf)=='2') && (CharBufferRead(fd,&buf)=='0') && (CharBufferRead(fd,&buf)=='0'));
	close(fd);
	return result;
}
void* HTTPChunkNewConnection(){return NULL;}
void HTTPChunkCloseConnection(void* x){}
