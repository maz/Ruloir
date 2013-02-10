#ifndef CONFIG_H
#define CONFIG_H

#include "prefix.h"

struct RuloirConfiguration{
	const char *system_id;
	const char *redis_ip;
	const char *security_token;
	const char *bind;
	const char *app_path;
	int redis_port;
	int port;
	int max_waiting_clients;
	int client_queue_length;
	int chunk_cache_length;
};
extern struct RuloirConfiguration Configuration;

void ConfigurationLoad(const char *config_file);

#endif