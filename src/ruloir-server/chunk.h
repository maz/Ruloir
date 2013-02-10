#ifndef CHUNK_H
#define CHUNK_H

#include "prefix.h"

typedef struct Chunk{
	char *key_a;
	char *key_b;
	int len;
	char *value;
} Chunk;

extern void (*ChunkGet)(void*,Chunk* chunk);

//This will take only the key_a part, the idea is not to check the existance of a particular entity with a certain ID, not to check it's validity and completeness
extern bool (*ChunkExists)(void*,const char *key);
extern void* (*ChunkNewConnection)();
extern void (*ChunkCloseConnection)(void*);

bool ChunkBackendLoad();

#endif
