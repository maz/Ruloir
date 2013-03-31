#include "char-buffer.h"
#include "atomics.h"

#define CHAR_BUFFER_SIZE	(2048)
struct CharBuffer{
	bool used;
	CharBuffer *next;
	int idx;
	int len;
	char buffer[CHAR_BUFFER_SIZE];
};

static CharBuffer first_char_buffer={
	false,
	&first_char_buffer,
	0
};
static CharBuffer *head=&first_char_buffer;
static pthread_mutex_t insertion_mutex=PTHREAD_MUTEX_INITIALIZER;

CharBuffer* CharBufferNew(){
	CharBuffer *nde=head;
	bool failure=true;
	do{
		if(CompareAndSwap(&nde->used, false, true)==false/*since it returns the old*/){
			failure=false;
			//As long as head ends up as one of the nodes, it's okay, but this line should speed things up a bit.
			head=head->next;
			break;
		}
	}while(nde!=head);
	if(failure){
		nde=malloc(sizeof(CharBuffer));
		nde->used=true;
		pthread_mutex_lock(&insertion_mutex);
		nde->next=head->next;
		head->next=nde;
		pthread_mutex_unlock(&insertion_mutex);
	}
	nde->idx=0;
	nde->len=0;
	return nde;
}
void CharBufferFree(CharBuffer *buf){
	buf->used=false;
}

char CharBufferRead(int fd,CharBuffer *buf){
	if(buf->len<=0 || buf->idx==buf->len){
		buf->len=read(fd,buf->buffer,CHAR_BUFFER_SIZE);
		buf->idx=0;
		if(buf->len==0)
			return -1;
	}
	return buf->buffer[(buf->idx++)];
}

bool CharBufferEof(CharBuffer *buf){
	return buf->len>0;
}

void CharBufferReadMany(int fd, CharBuffer *buf, size_t len, void* dst){
	size_t num=(buf->len-buf->idx);
	if(len>num){
		memcpy(dst, &buf->buffer[buf->idx], num);
		len-=num;
		buf->idx=buf->len;
		dst+=num;
		while(len>0){
			num=read(fd,dst,len);
			len-=num;
			dst+=num;
		}
	}else{
		memcpy(dst, &buf->buffer[buf->idx], len);
		buf->idx+=len;
	}
}