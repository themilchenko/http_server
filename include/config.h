#ifndef CONFIG_H
#define CONFIG_H

struct cfg_t {
    int port;
    int cpu_limit;
    int thread_limit;
    char document_root[1024];
};

void init_config(struct cfg_t *config);

int parse_config(const char *path, struct cfg_t *config);

#endif // CONFIG_H
