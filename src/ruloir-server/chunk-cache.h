#ifndef CHUNK_CACHE_H
#define CHUNK_CACHE_H

#include "prefix.h"
#include "chunk.h"

typedef struct ChunkCache{
	Chunk* chunks;
	void *connection;
} ChunkCache;

bool ChunkCacheExists(ChunkCache cache,const char *key);
Chunk* ChunkCacheGet(ChunkCache cache,const char *a,const char *b);
Chunk* ChunkCacheLoadKey(ChunkCache cache,const char *a,const char *b);
void ChunkCacheInit(ChunkCache *cache);

#endif