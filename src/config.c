#include "config.h"
#include "log.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define MAX_KEY_LENGTH 128
#define MAX_VALUE_LENGTH 128

struct config_var{
	char key[MAX_KEY_LENGTH];
	char value[MAX_VALUE_LENGTH];
};

static struct config_var
config_defaults[] = {
	// command line
	{"config", "config.lua"},

	// server
	{"sv_name", "Kaplar"},
	{"sv_addr", "127.0.0.1"},

	{"sv_echo_port", "7777"},
	{"sv_login_port", "7171"},
	{"sv_info_port", "7171"},
	{"sv_game_port", "7172"},

	// game
	{"tick_interval", "50"},

	// pgsql
	{"pgsql_host", "localhost"},
	{"pgsql_port", "5432"},
	{"pgsql_dbname", "kaplar"},
	{"pgsql_user", "admin"},
	{"pgsql_password", "admin"},
	{"pgsql_connect_timeout", "5"},
	{"pgsql_client_encoding", "UTF8"},
	{"pgsql_application_name", "kaplarc"},

	// misc
	{"motd", "1\nKaplar!"},
};

// config hash table
#define MAX_COLLISIONS 2
#define MAX_CONFIG_VARS 256
static struct config_var config_table[MAX_CONFIG_VARS];

static INLINE
uint32 config_var_idx(const char *key, size_t len){
	return murmur2_32((const uint8*)key, len, 0xC2EDF02D) % MAX_CONFIG_VARS;
}

static struct config_var *config_var_find(const char *key, size_t keylen){
	uint32 collisions = 0;
	uint32 idx = config_var_idx(key, keylen);
	do{
		if(strncmp(config_table[idx].key, key, keylen) == 0)
			return &config_table[idx];
		collisions += 1;
		idx += 1;
		if(idx >= MAX_CONFIG_VARS)
			idx = 0;
	}while(collisions < MAX_COLLISIONS);
	return NULL;
}

static struct config_var *config_var_insert(const char *key, size_t keylen){
	uint32 collisions = 0;
	uint32 idx = config_var_idx(key, keylen);
	do{
		if(config_table[idx].key[0] == 0)
			return &config_table[idx];
		DEBUG_LOG("config_var_insert: name collision with"
			" `%s` and `%s`", key, config_table[idx].key);
		collisions += 1;
		idx += 1;
		if(idx >= MAX_CONFIG_VARS)
			idx = 0;
	}while(collisions < MAX_COLLISIONS);
	return NULL;
}

static void config_var_set_value(struct config_var *var,
			const char *val, size_t vallen){
	if(vallen >= MAX_VALUE_LENGTH)
		vallen = MAX_VALUE_LENGTH - 1;
	memcpy(&var->value, val, vallen);
	var->value[vallen] = 0; // add nul-terminator
}

static void parse_argv(const char *argv, size_t *keylen,
		const char **val, size_t *vallen){
	const char *p;
	// parse key until nul-terminator or equal-sign
	p = argv;
	while(*p != 0 && *p != '=')
		p += 1;
	*keylen = (size_t)(p - argv);
	// parse val until nul-terminator
	if(*p == 0){
		*val = p;
		*vallen = 0;
	}else{
		p += 1; // skip equal-sign
		*val = p;
		while(*p != 0)
			p += 1;
		*vallen = p - *val;
	}
}

void config_init(int argc, char **argv){
	struct config_var *var;
	const char *key, *val;
	size_t keylen, vallen;
	int i;

	// zero all config vars
	memset(config_table, 0, sizeof(config_table));
	// load in defaults
	for(int i = 0; i < ARRAY_SIZE(config_defaults); i += 1){
		key = config_defaults[i].key;
		var = config_var_insert(key, strlen(key));
		if(var == NULL){
			LOG_WARNING("config_init: unable to insert `%s` into "
				" config table (too many collisions)", key);
			continue;
		}
		memcpy(var, &config_defaults[i], sizeof(struct config_var));
	}
	// parse cmdline
	for(i = 1; i < argc; i += 1){
		key = argv[i];
		parse_argv(key, &keylen, &val, &vallen);
		if(keylen == 0)
			continue;
		var = config_var_find(key, keylen);
		if(var == NULL){
			LOG_WARNING("config_init: invalid config var"
				" from cmdline `%.*s`", keylen, key);
			continue;
		}
		if(vallen == 0)
			config_var_set_value(var, "true", 4);
		else
			config_var_set_value(var, val, vallen);
	}
}

bool config_load(void){
	const char *config = config_get("config");
	if(*config == 0)
		return false;
	return config_load_from_path(config);
}

bool config_load_from_path(const char *path){
	lua_State *L;
	struct config_var *var;
	const char *key, *val;
	size_t vallen;
	int i;

	L = luaL_newstate();
	if(L == NULL)
		return false;
	if(luaL_loadfile(L, path) != 0){
		LOG_ERROR("config_load: failed to load config `%s`", path);
		lua_close(L);
		return false;
	}
	if(lua_pcall(L, 0, 0, 0) != 0){
		LOG_ERROR("config_load: %s", lua_tostring(L, -1));
		lua_close(L);
		return false;
	}
	for(i = 0; i < ARRAY_SIZE(config_defaults); i++){
		key = config_defaults[i].key;
		var = config_var_find(key, strlen(key));
		if(var == NULL)
			continue;
		lua_getglobal(L, key);
		if(lua_isstring(L, -1) != 0){
			val = lua_tolstring(L, -1, &vallen);
			config_var_set_value(var, val, vallen);
		}
		lua_pop(L, 1);
	}

	/*
	// NOTE: without any luaopen_* functions the only thing on
	// the global scope are config variables
	lua_pushnil(L);
	while(lua_next(L, LUA_GLOBALSINDEX) != 0){
		LOG("%s = %s",
			lua_tostring(L, -2),
			lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	*/

	lua_close(L);
	return true;
}

const char *config_get(const char *key){
	struct config_var *var = config_var_find(key, strlen(key));
	if(var != NULL)
		return var->value;
	LOG_WARNING("config_get: trying to fetch invalid variable `%s`", key);
	return "";
}

bool config_getb(const char *key){
	const char *value = config_get(key);
	return strcmp(value, "false") != 0;
}

int config_geti(const char *key){
	const char *value = config_get(key);
	return (int)strtol(value, NULL, 10);
}

float config_getf(const char *key){
	const char *value = config_get(key);
	return strtof(value, NULL);
}
