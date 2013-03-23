#include "chunk-cache.h"
#include "config.h"
#include "utils.h"
#include "log.h"

bool ChunkCacheExists(ChunkCache cache,const char *key){
	for(unsigned int i=0;i<Configuration.chunk_cache_length;i++){
		if(streq(cache.chunks[i].key_a,key)){
			return true;
		}
	}
	return ChunkExists(cache.connection,key);
}
Chunk* ChunkCacheGet(ChunkCache cache,const char *a,const char *b){
	for(unsigned i=0;i<Configuration.chunk_cache_length;i++){
		if(streq(cache.chunks[i].key_a,a) && streq(cache.chunks[i].key_b,b)){
			if(LogLevelMinimum<=LOG_LEVEL_DEBUG){//performance reasons
				LogEntryBegin(LOG_LEVEL_DEBUG);
				LogEntryPutString("Cache hit for thread ");
				LogEntryPutPthreadSelf();
				LogEntryPutString(" key: ");
				LogEntryPutString(a);
				if(b){
					LogEntryPutString("[");
					LogEntryPutString(b);
					LogEntryPutString("]");
				}
			}
			return &cache.chunks[i];
		}
	}
	return ChunkCacheLoadKey(cache,a,b);
}
static void LogCacheMiss(const char *a, const char *b){
	LogEntryBegin(LOG_LEVEL_DEBUG);
	LogEntryPutString("Cache miss for thread ");
	LogEntryPutPthreadSelf();
	LogEntryPutString(" key: ");
	LogEntryPutString(a);
	if(b){
		LogEntryPutString("[");
		LogEntryPutString(b);
		LogEntryPutString("]");
	}
}
Chunk* ChunkCacheLoadKey(ChunkCache cache,const char *a,const char *b){
#define LOAD_CACHE()	({cache.chunks[i].key_a=strdup(a);							\
						cache.chunks[i].key_b=strdup(b);							\
						if(LogLevelMinimum<=LOG_LEVEL_DEBUG){LogCacheMiss(a,b);}	\
						ChunkGet(cache.connection,&cache.chunks[i]);				\
						return &cache.chunks[i];			})
	unsigned int i;
	for(i=0;i<Configuration.chunk_cache_length;i++){
		if(!cache.chunks[i].key_a){
			LOAD_CACHE();
		}
	}
	//TODO: make a better caching precedence algorthim that ensures that no requested chunks get freed until the end of the request
	i=Configuration.chunk_cache_length-1;
	free(cache.chunks[i].key_a);
	free(cache.chunks[i].key_b);
	free(cache.chunks[i].value);
	LOAD_CACHE();
}
void ChunkCacheInit(ChunkCache *cache){
	cache->connection=ChunkNewConnection();
	cache->chunks=calloc(Configuration.chunk_cache_length,sizeof(Chunk));
}