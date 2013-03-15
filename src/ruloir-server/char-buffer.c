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