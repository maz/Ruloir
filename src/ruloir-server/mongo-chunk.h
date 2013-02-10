#ifndef MONGO_CHUNK
#define MONGO_CHUNK
#include "prefix.h"
#include "chunk.h"

#ifdef MONGO_CLIENT_INSTALLED

void MongoChunkGet(void*,Chunk *chunk);
bool MongoChunkExists(void*,const char *key);
void* MongoChunkNewConnection();
void MongoChunkCloseConnection(void*);

#endif

#endif