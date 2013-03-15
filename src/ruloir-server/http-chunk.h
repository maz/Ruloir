#ifndef HTTP_CHUNK_H
#define HTTP_CHUNK_H

#include "chunk.h"

void HTTPChunkGet(void*,Chunk *chunk);
bool HTTPChunkExists(void*,const char *key);
void* HTTPChunkNewConnection();
void HTTPChunkCloseConnection(void*);

#endif
