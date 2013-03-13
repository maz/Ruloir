#include "mongo-chunk.h"

#ifdef MONGO_CLIENT_INSTALLED

void MongoChunkGet(void* conn, Chunk *chunk){
	bson query[1];
	bson fields[1];
	bson result[1];
	
	mongo_cursor cursor[1];
	
	bson_init(query);
		bson_append_string(query, Configuration.mongo_key, chunk.key);
	bson_finish(query);
	
	bson_destroy(query);
	mongo_cursor_destroy(cursor);
}
bool MongoChunkExists(void* conn, const char *key){
	return false;
}
void* MongoChunkNewConnection(){
	mongo *conn=malloc(sizeof(mongo));
	mongo_init(conn);
	mongo_set_op_timeout(conn, Configuration.mongo_timeout);
	int status=mongo_client(conn, Configuration.mongo_server, Configuration.mongo_port);
	if(status != MONGO_OK){
		//TODO: panic
	}
	return conn;
}
void MongoChunkCloseConnection(void* conn){
	mongo_destroy(conn);
	free(conn);
}

#endif