#include "chunk.h"
#include "utils.h"
#include "redis-chunk.h"
#include "mongo-chunk.h"
#include "config.h"

void (*ChunkGet)(void*,Chunk* chunk);
bool (*ChunkExists)(void*,const char *key);
void* (*ChunkNewConnection)();
void (*ChunkCloseConnection)(void*);

struct ChunkBackend{
	const char *name;
	void (*ChunkGet)(void*,Chunk* chunk);
	bool (*ChunkExists)(void*,const char *key);
	void* (*ChunkNewConnection)();
	void (*ChunkCloseConnection)(void*);
};

static const struct ChunkBackend chunk_backends[]={
	{"redis",RedisChunkGet,RedisChunkExists,RedisChunkNewConnection,RedisChunkCloseConnection},
	#ifdef MONGO_CLIENT_INSTALLED
	{"mongo",MongoChunkGet,MongoChunkExists,MongoChunkNewConnection,MongoChunkCloseConnection},
	#endif
	{NULL}
};

bool ChunkBackendLoad(){
	const struct ChunkBackend *backend=&chunk_backends[0];
	while(backend->name){
		if(streq_ncs(backend->name,Configuration.chunk_backend)){
			ChunkGet=backend->ChunkGet;
			ChunkExists=backend->ChunkExists;
			ChunkNewConnection=backend->ChunkNewConnection;
			ChunkCloseConnection=backend->ChunkCloseConnection;
			return true;
		}
		++backend;
	}
	return false;
}