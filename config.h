#ifndef CONFIG_H
#define CONFIG_H

#include "prefix.h"

struct RuloirConfiguration{
	const char *system_id;
	const char *redis_ip;
	const char *security_token;
	const char *bind;
	int redis_port;
	int port;
	int max_waiting_clients;
};
extern struct RuloirConfiguration Configuration;

void ConfigurationLoad(const char *config_file);

#endif