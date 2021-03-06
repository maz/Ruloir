#include "app.h"
#include "utils.h"
#include "chunk-cache.h"

App* AppOpen(const char *file,char **err){
	void *handle=dlopen(file,RTLD_NOW);
	if(!handle){
		*err=strdup(dlerror());
		return NULL;
	}
	void *func=dlsym(handle,"AppFunc");
	if(!func){
		*err=strdup(dlerror());
		dlclose(handle);
		return NULL;
	}
	App *app=malloc(sizeof(App));
	app->func=func;
	app->handle=handle;
	app->create_handle=dlsym(handle, "AppCreateHandle");
	app->destroy_handle=dlsym(handle, "AppDestroyHandle");
	app->ref_count=0;//Yes, that's right
	if((func=dlsym(handle, "AppInit"))){
		((void (*)())func)();
	}
	return app;
}
App* AppRetain(App* app){
	if(app){
		++app->ref_count;
	}
	return app;
}
void AppRelease(App *app){
	--app->ref_count;
	if(app->ref_count<=0){
		void *func=dlsym(app->handle, "AppClose");
		if(func){
			((void (*)())func)();
		}
		dlclose(app->handle);
		free(app);
	}
}

void AppChunkGet(void *ctx,const char *key_a,const char *key_b,const char **ptr,int *len){
	Chunk *chunk=ChunkCacheGet(*((ChunkCache*)ctx),key_a,key_b);
	*len=chunk->len;
	*ptr=chunk->value;
}
bool AppChunkExists(void* ctx,const char *key){
	return ChunkCacheExists(*((ChunkCache*)ctx),key);
}