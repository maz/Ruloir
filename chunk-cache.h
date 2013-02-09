#ifndef CHUNK_CACHE_H
#define CHUNK_CACHE_H

#include "prefix.h"
#include "chunk.h"

typedef Chunk* ChunkCache;

Chunk* ChunkCacheGet(ChunkCache cache,const char *a,const char *b);
Chunk* ChunkCacheLoadKey(ChunkCache cache,const char *a,const char *b);
void ChunkCacheInit(ChunkCache *cache);

#endif