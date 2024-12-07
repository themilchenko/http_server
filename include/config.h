#ifndef CONFIG_H
#define CONFIG_H

# include "server.h"

typedef struct cfg_t {
    int port;
    int cpu_limit;
    char document_root[1024];
    char ssl_cert[MAX_PATH_LEN];   // Путь к SSL сертификату
    char ssl_key[MAX_PATH_LEN];
} cfg_t;

int init_config(cfg_t *config);

int parse_config(const char *path, cfg_t *config);

#endif // CONFIG_H
