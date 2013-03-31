#ifndef CONFIG_H
#define CONFIG_H

#include "prefix.h"

struct RuloirConfiguration{
#define CONFIG(key,parse_type,type,name,default)	type name;
	#include "config-keys.h"
#undef CONFIG
	char overflow;
};
extern struct RuloirConfiguration Configuration;

void ConfigurationLoad(const char *config_file);

#endif