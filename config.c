#include "config.h"
#include "prefix.h"

struct RuloirConfiguration Configuration={
	.system_id="SYSID",
	.redis_ip="localhost",
	.redis_port=6379,
	.port=8080,
	.bind="::1",
	.max_waiting_clients=20
};

enum{
	CONFIG_STATE_KEY=1,
	CONFIG_STATE_VALUE
};

static void StrAppend(char** str,char ch){
	size_t l=strlen(*str);
	*str=realloc(*str,l+2);
	(*str)[l+1]=0;
	(*str)[l]=ch;
}

static struct ConfigKeyValue{
	const char *key;
	bool convert_to_int;
	void* ptr;
} config_keys_values[]={
	{"systemid",false,&Configuration.system_id},
	{"redisip",false,&Configuration.redis_ip},
	{"redisport",true,&Configuration.redis_port},
	{"port",true,&Configuration.port},
	{"securitytoken",false,&Configuration.security_token},
	{"bind",false,&Configuration.bind},
	{NULL}
};

static void SetConfig(char *key,char *value){
	size_t len=strlen(key);
	int i;
	for(i=0;i<len;i++){
		key[i]=tolower(key[i]);
	}
	for(i=len-1;i>=0;i--){
		if(isspace(key[i]))
			key[i]=0;
		else
			break;
	}
	for(i=0;i<len;i++){
		if(!isspace(key[i]))
			break;
	}
	if(i>=len)
		return;
	struct ConfigKeyValue *kv=&config_keys_values[0];
	while(kv->key){
		if(strcmp(kv->key,key+i)==0){
			while(*value && isspace(*value)){
				++value;
			}
			char *str_ptr=value;
			while(*str_ptr){
				if(isspace(*str_ptr))
					*str_ptr=0;
				++str_ptr;
			}
			if(*value){
				if(kv->convert_to_int){
					*((int*)kv->ptr)=(int)strtol(value,NULL,10);
				}else{
					*((char**)kv->ptr)=strdup(value);
				}
			}
			break;
		}
		++kv;
	}
}

void ConfigurationLoad(const char *config_file){
#define SET_CONFIGURATION_VALUE()	({SetConfig(key,value);		\
				free(key);										\
				free(value);									\
				key=malloc(sizeof(char));						\
				*key=0;											\
				value=malloc(sizeof(char));						\
				*value=0;})
	FILE *f=fopen(config_file,"r");
	if(!f){
		perror("ConfigurationLoad");
		exit(1);
	}
	char ch=fgetc(f);
	char state=CONFIG_STATE_KEY;
	char *key=malloc(sizeof(char));
	*key=0;
	char *value=malloc(sizeof(char));
	*key=0;
	while(ch && ch!=EOF){
		if(ch=='\n'){
			SET_CONFIGURATION_VALUE();
			state=CONFIG_STATE_KEY;
		}else if(state==CONFIG_STATE_KEY){
			if(ch==':'){
				state=CONFIG_STATE_VALUE;
			}else{
				StrAppend(&key,ch);
			}
		}else if(state==CONFIG_STATE_VALUE){
			StrAppend(&value,ch);
		}
		ch=fgetc(f);
	}
	if(state==CONFIG_STATE_VALUE){
		SET_CONFIGURATION_VALUE();
	}
	free(key);
	free(value);
}