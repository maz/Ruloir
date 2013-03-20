#ifndef LOG_H
#define LOG_H

#include "prefix.h"

enum{
	LOG_LEVEL_DEBUG=0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};

#define LAST_LOG_LEVEL		(LOG_LEVEL_FATAL)

typedef char			log_level_t;

void LogCreateThreadQueue();
bool LogOpen();
void LogClose();
void LogEntryBegin(log_level_t level);
void LogEntryPutString(const char *str);
void LogEntryPutNumber(long num);
void LogEntryPutHexRepr(void* data, size_t sze);
void LogEntryPutPthreadSelf();

#endif