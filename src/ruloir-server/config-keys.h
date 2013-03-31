//#define CONFIG(key,parse_type,type,name,default)

CONFIG("systemid",PARSE_TYPE_STRING,const char*,system_id,"SYSID")
CONFIG("securitytoken",PARSE_TYPE_STRING,const char*,security_token,"ABC123")
CONFIG("bind",PARSE_TYPE_STRING,const char*,bind,"::1")
CONFIG("apppath",PARSE_TYPE_STRING,const char*,app_path,"default-app")
CONFIG("chunkbackend",PARSE_TYPE_STRING,const char*,chunk_backend,"redis")
CONFIG("port",PARSE_TYPE_INT,int,port,8080)
CONFIG("maxwaitingclients",PARSE_TYPE_INT,int,max_waiting_clients,20)
CONFIG("clientqueuelength",PARSE_TYPE_INT,int,client_queue_length,20)
CONFIG("chunkcachelength",PARSE_TYPE_INT,int,chunk_cache_length,20)
CONFIG("defaulthandlerthreads",PARSE_TYPE_INT,int,default_handler_threads,1)

CONFIG("logfilepath",PARSE_TYPE_STRING,const char*,log_file_path,"./log.txt")
CONFIG("logfilestderrmirror",PARSE_TYPE_INT,int,log_file_stderr_mirror,1)
CONFIG("logfilecolor",PARSE_TYPE_INT,int,log_file_color,1)
CONFIG("loglevelminimum",PARSE_TYPE_STRING,const char*,log_level_minimum,"info")
CONFIG("logqueuelength",PARSE_TYPE_STRING,int,log_queue_length,100)
CONFIG("logfetchtimeinbackground",PARSE_TYPE_BOOL,bool,log_fetch_time_in_background,true)

CONFIG("redisport",PARSE_TYPE_INT,int,redis_port,6379)
CONFIG("redisip",PARSE_TYPE_STRING,const char*,redis_ip,"localhost")

CONFIG("httpport",PARSE_TYPE_INT,int,http_port,3000)
CONFIG("httpip",PARSE_TYPE_STRING,const char*,http_ip,"localhost")
CONFIG("httppathprefix",PARSE_TYPE_STRING,const char*,http_path_prefix,"/")
CONFIG("httppathdelimiter",PARSE_TYPE_STRING,const char*,http_path_delimeter,"/")
CONFIG("httppathsuffix",PARSE_TYPE_STRING,const char*,http_path_suffix,"")

#ifdef MONGO_CLIENT_INSTALLED
	CONFIG("mongotimeout",PARSE_TYPE_INT,int,mongo_timeout,1000)
	CONFIG("mongoserver",PARSE_TYPE_STRING,const char*,mongo_server,"127.0.0.1")
	CONFIG("mongoport",PARSE_TYPE_INT,int,mongo_port,27017)
	CONFIG("mongocollection",PARSE_TYPE_STRING,const char*,mongo_db,"test.ruloir")
	CONFIG("mongokeyfield",PARSE_TYPE_STRING,const char*,mongo_key_field,"key")
	CONFIG("mongovaluefield",PARSE_TYPE_STRING,const char*,mongo_value_field,"value")
#endif