#include <ruloir-prefix.h>

#define WriteConstStr(fd,str)	write(fd,str,(sizeof(str)/sizeof(char))-1)

void AppFunc(void* ctx,
			int fd,
			const char *method,
			const char *path,
			void (*ChunkGet)(void *ctx,const char *key_a,const char *key_b,const char **ptr,int *len),
			bool (*ChunkExists)(void* ctx,const char *key)
){
	WriteConstStr(fd,"HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
	const char *data;
	int len;
	ChunkGet(ctx,path+1,NULL,&data,&len);
	write(fd,data,len);
	/*if(ChunkExists(ctx,path+1)){
		WriteConstStr(fd, "true");
	}else{
		WriteConstStr(fd, "false");
	}*/
}