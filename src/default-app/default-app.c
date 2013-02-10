#include <ruloir-prefix.h>

void AppFunc(void* ctx,
			int fd,
			const char *method,
			const char *path,
			void (*ChunkGet)(void *ctx,const char *key_a,const char *key_b,const char **ptr,int *len),
			bool (*ChunkExists)(void* ctx,const char *key)
){
	
}