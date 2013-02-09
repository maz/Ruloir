#ifndef HTTP_PARSER
#define HTTP_PARSER
#include "prefix.h"

typedef const char* HTTPMethod;

static const char* const HTTPGet		="GET";
static const char* const HTTPHead		="HEAD";
static const char* const HTTPPost		="POST";
static const char* const HTTPPut		="PUT";
static const char* const HTTPDelete		="DELETE";
static const char* const HTTPTrace		="TRACE";
static const char* const HTTPOptions	="OPTIONS";
static const char* const HTTPConnect	="CONNECT";
static const char* const HTTPPatch		="PATCH";

#define MAX_HTTP_PATH_LENGTH	(1023)

typedef struct{
	HTTPMethod method;
	char path[MAX_HTTP_PATH_LENGTH+1];
} HTTPRequest;

bool HTTPParse(int fd,HTTPRequest *http);

#endif