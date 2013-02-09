#ifndef CHUNK_H
#define CHUNK_H

typedef struct Chunk{
	char *key_a;
	char *key_b;
	int len;
	char *value;
} Chunk;

void ChunkGet(Chunk* chunk);

//This will take only the key_a part, the idea is not to check the existance of a particular entity with a certain ID, not to check it's validity and completeness
bool ChunkExists(const char *key);

#endif
