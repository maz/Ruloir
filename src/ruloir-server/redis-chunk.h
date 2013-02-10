#ifndef REDIS_CHUNK_H
#define REDIS_CHUNK_H

#include "chunk.h"

void RedisChunkGet(void*,Chunk *chunk);
bool RedisChunkExists(void*,const char *key);
void* RedisChunkNewConnection();
void RedisChunkCloseConnection(void*);

#endif