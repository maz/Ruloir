#include "chunk.h"
#include "utils.h"
#include "redis-chunk.h"
#include "config.h"

void (*ChunkGet)(Chunk* chunk);
bool (*ChunkExists)(const char *key);

struct ChunkBackend{
	const char *name;
	void (*ChunkGet)(Chunk* chunk);
	bool (*ChunkExists)(const char *key);
};

static const struct ChunkBackend chunk_backends[]={
	{"redis",RedisChunkGet,RedisChunkExists},
	{NULL}
};

bool ChunkBackendLoad(){
	const struct ChunkBackend *backend=&chunk_backends[0];
	while(backend->name){
		if(streq_ncs(backend->name,Configuration.chunk_backend)){
			ChunkGet=backend->ChunkGet;
			ChunkExists=backend->ChunkExists;
			return true;
		}
		++backend;
	}
	return false;
}