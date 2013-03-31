#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H

#include "prefix.h"

typedef struct CharBuffer CharBuffer;

CharBuffer* CharBufferNew();
void CharBufferFree(CharBuffer *buf);
bool CharBufferEof(CharBuffer *buf);
char CharBufferRead(int fd,CharBuffer *buf);
void CharBufferReadMany(int fd, CharBuffer *buf, size_t len, void* dst);

#endif