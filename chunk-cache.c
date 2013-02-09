#include "chunk-cache.h"
#include "config.h"
#include "utils.h"

bool ChunkCacheExists(ChunkCache cache,const char *key){
	for(unsigned int i=0;i<Configuration.chunk_cache_length;i++){
		if(streq(cache[i].key_a,key)){
			return true;
		}
	}
	return ChunkExists(key);
}
Chunk* ChunkCacheGet(ChunkCache cache,const char *a,const char *b){
	for(unsigned i=0;i<Configuration.chunk_cache_length;i++){
		if(streq(cache[i].key_a,a) && streq(cache[i].key_b,b)){
			return &cache[i];
		}
	}
	return ChunkCacheLoadKey(cache,a,b);
}
Chunk* ChunkCacheLoadKey(ChunkCache cache,const char *a,const char *b){
#define LOAD_CACHE()	({cache[i].key_a=strdup(a);	\
						cache[i].key_b=strdup(b);	\
						ChunkGet(&cache[i]);		\
						return &cache[i];			})
	unsigned int i;
	for(i=0;i<Configuration.chunk_cache_length;i++){
		if(!cache[i].key_a){
			LOAD_CACHE();
		}
	}
	i=Configuration.chunk_cache_length-1;
	free(cache[i].key_a);
	free(cache[i].key_b);
	free(cache[i].value);
	LOAD_CACHE();
}
void ChunkCacheInit(ChunkCache *cache){
	*cache=calloc(Configuration.chunk_cache_length,sizeof(Chunk));
}