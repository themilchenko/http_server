#include <sys/stat.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

// #include "server.h"
#include "utils.h"

int is_dir(char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) == 0) {
        return S_ISDIR(path_stat.st_mode);
    }
    return 0;
}

char *get_content_type(char *path) {
    char *extension = strrchr(path, '.');
    
    if (extension == NULL) {
        return "text/plain";
    }
    if (strcmp(extension, ".html") == 0) {
        return "text/html";
    }
    if (strcmp(extension, ".css") == 0) {
        return "text/css";
    }
    if (strcmp(extension, ".js") == 0) {
        return "application/javascript";
    }
    if (strcmp(extension, ".jpeg") == 0) {
        return "image/jpeg";
    }
    if (strcmp(extension, ".png") == 0) {
        return "image/png";
    }
    if (strcmp(extension, ".gif") == 0) {
        return "image/gif";
    }
    if (strcmp(extension, ".swf") == 0) {
        return "application/x-shockwave-flash";
    }
    return "text/plain";
}

void hex_to_ascii(char *hex_str, char *ascii_str) {
    size_t j = 0;
    for (size_t i = 0; i < strlen(hex_str); i++) {
        if (hex_str[i] == '%') {
            if (i + 2 < strlen(hex_str) && isxdigit(hex_str[i+1]) && isxdigit(hex_str[i+2])) {
                char hex[3] = { hex_str[i+1], hex_str[i+2], '\0' };
                int value = strtol(hex, NULL, 16);
                ascii_str[j++] = value;
                i += 2; // go to next hex pair
            } else {
                // invalid hex format, so copy as is
                ascii_str[j++] = hex_str[i];
            }
        } else {
            // not a hex character, so copy as is
            ascii_str[j++] = hex_str[i];
        }
    }
    ascii_str[j] = '\0'; // terminate the string
}
