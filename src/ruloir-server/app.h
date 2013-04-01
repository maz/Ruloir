#ifndef APP_H
#define APP_H

#include "prefix.h"
#include "http-parser.h"
#include "chunk-cache.h"

void AppChunkGet(void *ctx,const char *key_a,const char *key_b,const char **ptr,int *len);
bool AppChunkExists(void* ctx,const char *key);

typedef struct App{
	void (*func)(void *handle,
				void* cache,
				int fd,
				const char *method,
				const char *path,
				void (*ChunkGet)(void *ctx,const char *key_a,const char *key_b,const char **ptr,int *len),
				bool (*ChunkExists)(void* ctx,const char *key)
	);
	void* (*create_handle)();
	void (*destroy_handle)(void*);
	int ref_count;
	void* handle;
} App;

App* AppOpen(const char *file,char **err);
App* AppRetain(App* app);
void AppRelease(App *app);

#endif