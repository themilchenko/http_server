#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"

int init_config(cfg_t *config) {
    config->port = 80;
    config->cpu_limit = 1;

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return -1;
    }

    strcpy(config->document_root, cwd);
    return 0;
}

int parse_config(const char *path, cfg_t *config) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == "") {
            continue;
        }

        line[strcspn(line, "\n")] = '\0';

        char key[256];
        char value[256];
        int count = sscanf(line, "%s %s", key, value);
        if (count != 2) {
            continue;
        }

        if (strcmp(key, "cpu_limit") == 0) {
            config->cpu_limit = atoi(value);
        } else if (strcmp(key, "port") == 0) {
            config->port = atoi(value);
        } else if (strcmp(key, "document_root") == 0) {
            strcpy(config->document_root, value);
            if (access(config->document_root, R_OK) != 0) {
                perror("access");
                return EXIT_FAILURE;
            }
        }
    }

    return 0;
}
