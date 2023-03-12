#include "utils.h"
#include <sys/stat.h>
#include <string.h>
#include <stddef.h>

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
