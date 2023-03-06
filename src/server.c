#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include "server.h"

#define PORT 8080
#define BACKLOG 10

void handle_request(int client_socket) {
    char request_str[MAX_REQUEST_LEN];
    ssize_t bytes_recieved = recv(client_socket, request_str, MAX_REQUEST_LEN - 1, 0);
    if (bytes_recieved < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    request_str[bytes_recieved] = '\0';

    struct http_request_t request;
    parse_request(request_str, &request);

    struct http_response_t response;
    if (strcmp(request.method, "GET") != 0) {
        strcpy(response.status, "HTTP/1.1 405 Method Not Allowed\r\n");
        strcpy(response.content_type, "Content-Type: text/html\r\n");
        strcpy(response.body, "<html><body><h1>405 Method Not Allowed</h1></body></html>");

        send_response(client_socket, &response);
        return;
    }

    char path[MAX_PATH_LEN];
    snprintf(path, MAX_PATH_LEN, ".%s", request.path);
    if (access(path, F_OK) == -1) {
        strcpy(response.status, "HTTP/1.1 404 Not Found\r\n");
        strcpy(response.content_type, "Content-Type: text/html\r\n");
        strcpy(response.body, "<html><body><h1>404 Not Found</h1></body></html>");

        send_response(client_socket, &response);
        return;
    }

    char content_type[32];
    snprintf(content_type, 32, "Content-Type: %s\r\n", get_content_type(path));

    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    char *body = malloc(st.st_size);
    read_file(path, body, st.st_size);

    strcpy(response.status, "HTTP/1.1 200 OK\r\n");
    strcpy(response.content_type, content_type);
    strcpy(response.body, body);

    send_response(client_socket, &response);
}

void send_response(int client_socket, struct http_response_t* response) {
    char response_str[MAX_RESPONSE_LEN];
    size_t response_len = sprintf(response_str, MAX_RESPONSE_LEN, "%s%sContent-Length: %ld\r\n\r\n%s", response->status, response->content_type, strlen(response->body), response->body);
    ssize_t bytes_sent = send(client_socket, response_str, response_len, 0);
    if (bytes_sent < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

void parse_request(char *request_str, struct http_request_t *request) {
    char *saveptr;
    char *token = strtok_r(request_str, "\r\n", &saveptr);
    strncpy(request->method, token, 8);
    token = strtok_r(NULL, "\r\n", &saveptr);
    strncpy(request->path, token, MAX_PATH_LEN);
}

void read_file(char *path, char *buffer, ssize_t size) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_read = read(fd, buffer, size);
    if (bytes_read < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    close(fd);
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
    return "text/plain";
}
