#ifndef CHUNK_H
#define CHUNK_H

typedef struct Chunk{
	char *key_a;
	char *key_b;
	int len;
	char *value;
} Chunk;

void ChunkGet(Chunk* chunk);

#endif
