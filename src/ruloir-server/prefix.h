#ifndef RULOIR_PREFIX_H
#define RULOIR_PREFIX_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#define JEMALLOC_MANGLE		1
#include <jemalloc/jemalloc.h>
#include <pthread.h>
#ifdef MONGO_CLIENT_INSTALLED
#include <mongo.h>
#endif

#define LOGGING_INCLUDE_LOCATION	1

#define strdup(...)	CSTDLIB_STRDUP_IS_DANGEROUS_DUE_TO_JEMALLOC()

#endif
