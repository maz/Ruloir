#ifndef LOG_H
#define LOG_H

#include "prefix.h"

//We can't use enum here, since these need a known type
#define LOG_END				((int)0)
#define LOG_STRING			((int)1)
//TODO: implement LOG_HEXREPR
//#define LOG_HEXREPR			((int)2)
#define LOG_NUMBER			((int)3)

enum{
	LOG_LEVEL_DEBUG=0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};

#define LAST_LOG_LEVEL		(LOG_LEVEL_FATAL)

typedef char			log_level_t;
extern log_level_t LogLevelMinimum;

bool LogOpen();
void LogClose();
void Log_Internal(log_level_t log_level, ...);
#if defined(LOGGING_INCLUDE_LOCATION) && LOGGING_INLCUDE_LOCATION
#define Log(level, ...)		Log_Internal(__FILE__, __LINE__, __FUNCTION__, level, __VA_ARGS__)
#else
#define Log(level, ...)		Log_Internal(level, __VA_ARGS__)
#endif
#endif