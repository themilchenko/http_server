#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include "utils.h"

#define MAX_REQUEST_LEN 2048
#define MAX_PATH_LEN 1024
#define MAX_HEADER_LEN 2048
#define MAX_RESPONSE_LEN 102400

#define PORT 8081
#define BACKLOG 10

typedef struct http_request_t {
    char method[10];
    char path[MAX_PATH_LEN];
} http_request_t;

typedef struct http_response_t {
    int status_code;
    char content_type[32];
    char body[MAX_RESPONSE_LEN];

    off_t body_size;
    size_t header_size;
} http_response_t;

void handle_request(int client_socket);

#endif // SERVER_H
