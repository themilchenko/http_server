#ifndef CONFIG_H
#define CONFIG_H

typedef struct cfg_t {
    int port;
    int cpu_limit;
    char document_root[1024];
} cfg_t;

void init_config(cfg_t *config);

int parse_config(const char *path, cfg_t *config);

#endif // CONFIG_H
