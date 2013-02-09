#ifndef APP_H
#define APP_H

#include "prefix.h"
#include "http-parser.h"
#include "chunk-cache.h"

typedef struct App{
	void (*func)(int fd,HTTPRequest *http,ChunkCache cache);
	int ref_count;
	void* handle;
} App;

App* AppOpen(const char *file,char **err);
App* AppRetain(App* app);
void AppRelease(App *app);

#endif