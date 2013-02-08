#include "prefix.h"
#include "chunk.h"


void ChunkGet(Chunk *chunk){
	
}

void ChunkFreeContents(Chunk *chunk){
	free(chunk->key_a);
	free(chunk->key_b);
	free(chunk->value);
	memset(chunk,0,sizeof(Chunk));
}
