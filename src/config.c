#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void init_config(struct cfg_t *config) {
    config->port = 8080;
    config->cpu_limit = 1;
    strcpy(config->document_root, "var/www/html");
}

int parse_config(const char *path, struct cfg_t *config) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        char key[256];
        char value[256];
        int count = sscanf(line, "%s %s", key, value);
        if (count != 2) {
            continue;
        }

        if (strcmp(key, "cpu_limit") == 0) {
            config->cpu_limit = atoi(value);
        } else if (strcmp(key, "thread_limit") == 0) {
            config->thread_limit = atoi(value);
        } else if (strcmp(key, "port") == 0) {
            config->port = atoi(value);
        } else if (strcmp(key, "document_root") == 0) {
            strcpy(config->document_root, value);
        }
    }

    return EXIT_SUCCESS;
}
