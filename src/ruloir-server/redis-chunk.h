#ifndef REDIS_CHUNK_H
#define REDIS_CHUNK_H

#include "chunk.h"

void RedisChunkGet(Chunk *chunk);
bool RedisChunkExists(const char *key);

#endif