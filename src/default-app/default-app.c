#include <ruloir-prefix.h>

#define WriteConstStr(fd,str)	write(fd,str,(sizeof(str)/sizeof(char))-1)

void AppFunc(void* handle,
			void* cache,
			int fd,
			const char *method,
			const char *path,
			void (*ChunkGet)(void *cache,const char *key_a,const char *key_b,const char **ptr,int *len),
			bool (*ChunkExists)(void* cache,const char *key)
){
	WriteConstStr(fd,"HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\n");
	const char *data;
	int len;
	ChunkGet(cache,path+1,NULL,&data,&len);
	write(fd,data,len);
	/*if(ChunkExists(cache,path+1)){
		WriteConstStr(fd, "true");
	}else{
		WriteConstStr(fd, "false");
	}*/
}