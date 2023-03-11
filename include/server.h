#ifndef SERVER_H
#define SERVER_H

#define MAX_REQUEST_LEN 2048
#define MAX_PATH_LEN 1024
#define MAX_HEADER_LEN 2048
#define MAX_RESPONSE_LEN 102400

#define PORT 8081
#define BACKLOG 10

struct http_request_t {
    char method[10];
    char path[MAX_PATH_LEN];
};

struct http_response_t {
    char status[32];
    char content_type[32];
    char body[MAX_RESPONSE_LEN];
};

void handle_request(int client_socket);
void send_response(int client_socket, struct http_response_t* response);
void parse_request(char* request_str, struct http_request_t* request);

void read_file(char *path, char *buffer, ssize_t size);
char *get_content_type(char *path);

#endif // SERVER_H
