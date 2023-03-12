#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#include "server.h"
#include "utils.h"

/*========================================PRIVATE=============================================*/

void send_response_with_buffer(int client_socket, struct http_response_t* response) {
    char response_str[MAX_RESPONSE_LEN];
    size_t response_len = snprintf(response_str, MAX_RESPONSE_LEN, "%s%sContent-Length: %ld\r\n\r\n%s", 
                                    response->status, response->content_type, response->body_size, response->body);

    ssize_t bytes_sent = send(client_socket, response_str, response_len, 0);
    if (bytes_sent < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

void parse_request(char *request_str, struct http_request_t *request) {
    char *saveptr;
    char *token = strtok_r(request_str, "\r\n", &saveptr);

    // Parsing method name
    token = strtok_r(token, " ", &saveptr);
    strncpy(request->method, token, 8);
    request->method[8] = '\0';

    // Parsing path name
    char *last_space = strrchr(saveptr, ' ');
    size_t path_len = last_space - saveptr;
    strncpy(request->path, saveptr, path_len);
    request->path[path_len] = '\0';
}

void send_response_with_file(int client_socket, struct http_request_t request, struct http_response_t* response) {
    char response_str[MAX_RESPONSE_LEN];
    size_t response_len = 0;

    response_len = snprintf(response_str, MAX_RESPONSE_LEN, "%s%sContent-Length: %ld\r\n\r\n", 
                                    response->status, response->content_type, response->body_size);
    send(client_socket, response_str, response_len, 0);

    int filefd = open(request.path, O_RDONLY);
    if (filefd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    off_t offset = 0;
    ssize_t bytes_sent = sendfile(client_socket, filefd, &offset, response->body_size);
    if (bytes_sent < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    close(filefd);
}

/*=========================================PUBLIC==============================================*/

void handle_request(int client_socket) {
    char request_str[MAX_REQUEST_LEN];
    ssize_t bytes_recieved = recv(client_socket, request_str, MAX_REQUEST_LEN - 1, 0);
    if (bytes_recieved < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    request_str[bytes_recieved] = '\0';

    printf(request_str);

    struct http_request_t request;
    parse_request(request_str, &request);

    struct http_response_t response;
    if (strcmp(request.method, "GET") != 0) {
        strcpy(response.status, "HTTP/1.1 405 Method Not Allowed\r\n");
        strcpy(response.content_type, "Content-Type: text/html\r\n");
        strcpy(response.body, "<html><body><h1>405 Method Not Allowed</h1></body></html>");

        send_response_with_buffer(client_socket, &response);
        return;
    }

    char path_buf[MAX_PATH_LEN];
    snprintf(path_buf, MAX_PATH_LEN, ".%s", request.path);
    if (is_dir(path_buf)) {
        if (path_buf[strlen(path_buf) - 1] != '/') {
            strcat(path_buf, "/");
        }
        strcat(path_buf, "index.html");
    }
    strncpy(request.path, path_buf, MAX_PATH_LEN);
    request.path[MAX_PATH_LEN - 1] = '\0';
    
    if (access(request.path, F_OK) == -1) {
        strcpy(response.status, "HTTP/1.1 404 Not Found\r\n");
        strcpy(response.content_type, "Content-Type: text/html\r\n");
        strcpy(response.body, "<html><body><h1>404 Not Found</h1></body></html>");

        send_response_with_buffer(client_socket, &response);
        return;
    }

    // Determine content type from file extension
    char content_type[32];
    snprintf(content_type, 32, "Content-Type: %s\r\n", get_content_type(request.path));

    // Getting file size
    struct stat st;
    if (stat(request.path, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    response.body_size = st.st_size;

    // Setting response status and content type
    strcpy(response.status, "HTTP/1.1 200 OK\r\n");
    strcpy(response.content_type, content_type);

    send_response_with_file(client_socket, request, &response);
}
