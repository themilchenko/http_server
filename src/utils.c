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

// char *set_http_header(http_response_t *response) {
//     char result_header[MAX_HEADER_LEN];

//     char header[MAX_HEADER_LEN];
//     switch (response->status_code) {
//         case HTTP_STATUS_OK:
//             strncpy(header, HTTP_OK, MAX_HEADER_LEN);
//             break;
//         case HTTP_STATUS_NOT_FOUND:
//             strncpy(header, HTTP_NOT_FOUND, MAX_HEADER_LEN);
//             break;
//         case HTTP_STATUS_METHOD_NOT_ALLOWED:
//             strncpy(header, HTTP_NOT_ALLOWED, MAX_HEADER_LEN);
//             break;
//         default:
//             strncpy(header, HTTP_FORBIDDEN, MAX_HEADER_LEN);
//             break;
//     }
//     header[MAX_HEADER_LEN - 1] = '\0';

//     size_t recieved_bytes = snprintf(result_header, MAX_HEADER_LEN,
//                                      HTTP_RESPONSE_TEMPLATE,
//                                      header,
//                                      response->content_type,
//                                      response->body_size);
//     response->header_size = recieved_bytes;

//     return result_header;
// }
