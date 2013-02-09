#include "app.h"
#include "utils.h"

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
	app->ref_count=0;//Yes, that's right
	return app;
}
App* AppRetain(App* app){
	++app->ref_count;
	return app;
}
void AppRelease(App *app){
	--app->ref_count;
	if(app->ref_count<=0){
		dlclose(app->handle);
		free(app);
	}
}