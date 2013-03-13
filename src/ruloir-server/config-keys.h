//#define CONFIG(key,is_int,type,name,default)

CONFIG("systemid",false,const char*,system_id,"SYSID")
CONFIG("redisip",false,const char*,redis_ip,"localhost")
CONFIG("securitytoken",false,const char*,security_token,"ABC123")
CONFIG("bind",false,const char*,bind,"::1")
CONFIG("apppath",false,const char*,app_path,"default-app")
CONFIG("chunkbackend",false,const char*,chunk_backend,"redis")
CONFIG("redisport",true,int,redis_port,6379)
CONFIG("port",true,int,port,8080)
CONFIG("maxwaitingclients",true,int,max_waiting_clients,20)
CONFIG("clientqueuelength",true,int,client_queue_length,20)
CONFIG("chunkcachelength",true,int,chunk_cache_length,20)

#ifdef MONGO_CLIENT_INSTALLED
	CONFIG("mongotimeout",true,int,mongo_timeout,1000)
	CONFIG("mongoserver",false,const char*,mongo_server,"127.0.0.1")
	CONFIG("mongoport",true,int,mongo_port,27017)
	CONFIG("mongocollection",false,const char*,mongo_db,"test.ruloir")
	CONFIG("mongokeyfield",false,const char*,mongo_key_field,"key")
	CONFIG("mongovaluefield",false,const char*,mongo_value_field,"value")
#endif