#include "char-buffer.h"

char CharBufferRead(int fd,CharBuffer *buf){
	if(buf->len<=0 || buf->idx==buf->len){
		buf->len=read(fd,buf->buffer,CHAR_BUFFER_SIZE);
		buf->idx=0;
		if(buf->len==0)
			return -1;
	}
	return buf->buffer[(buf->idx++)];
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