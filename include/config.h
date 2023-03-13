#ifndef CONFIG_H
#define CONFIG_H

#define CPU_LIMIT 10

struct cfg_t {
    int port;
    int cpu_limit;
    char document_root[1024];
};

void init_config(struct cfg_t *config);

int parse_config(const char *path, struct cfg_t *config);

#endif // CONFIG_H
