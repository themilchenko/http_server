#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#include "server.h"
#include "utils.h"

/*========================================PRIVATE=============================================*/

void init_response(http_response_t *response) {
    response->header_size = 0;
    response->body_size = 0;
}

void set_header_response(http_response_t *response, const int status_code, const char *content_type) {
    response->status_code = status_code;
    strncpy(response->content_type, content_type, 32);
    response->content_type[31] = '\0';
}

void set_body_response(http_response_t *response, const char *body) {
    strncpy(response->body, body, MAX_RESPONSE_LEN);
    response->body[MAX_RESPONSE_LEN - 1] = '\0';
    response->body_size = strlen(response->body);
}

void set_http_header(http_response_t *response, char *response_str) {
    char result_header[MAX_HEADER_LEN];

    char header[MAX_HEADER_LEN];
    switch (response->status_code) {
        case HTTP_STATUS_OK:
            strncpy(header, HTTP_OK, MAX_HEADER_LEN);
            break;
        case HTTP_STATUS_NOT_FOUND:
            strncpy(header, HTTP_NOT_FOUND, MAX_HEADER_LEN);
            break;
        case HTTP_STATUS_METHOD_NOT_ALLOWED:
            strncpy(header, HTTP_NOT_ALLOWED, MAX_HEADER_LEN);
            break;
        default:
            strncpy(header, HTTP_FORBIDDEN, MAX_HEADER_LEN);
            break;
    }
    header[MAX_HEADER_LEN - 1] = '\0';

    char date[50];
    get_current_time(date);

    size_t recieved_bytes = snprintf(result_header, MAX_HEADER_LEN,
                                     HTTP_RESPONSE_TEMPLATE,
                                     header,
                                     date,
                                     response->content_type,
                                     response->body_size);
    response->header_size = recieved_bytes;

    strncpy(response_str, result_header, MAX_HEADER_LEN);
    response_str[MAX_HEADER_LEN - 1] = '\0';
}

int parse_request(char *request_str, http_request_t *request, char *root_dir) {
    char *saveptr;
    char *token = strtok_r(request_str, "\r\n", &saveptr);

    if (token == NULL) {
        return -1;
    }

    // Parsing method name
    token = strtok_r(token, " ", &saveptr);
    strncpy(request->method, token, 8);
    request->method[8] = '\0';

    // Parsing path name

    // Firstly check query params
    char *query_params = strchr(saveptr, '?');
    if (query_params == NULL) {
        query_params = strrchr(saveptr, ' ');
    }

    // char *last_space = strrchr(saveptr, ' ');
    size_t path_len = query_params - saveptr;
    strncpy(request->path, saveptr, path_len);
    request->path[path_len] = '\0';

    // Check if path is hex encoded
    char hex_encoded[strlen(request->path)];
    hex_to_ascii(request->path, hex_encoded);
    strncpy(request->path, hex_encoded, MAX_PATH_LEN);
    request->path[MAX_PATH_LEN - 1] = '\0';

    // Concatenate root dir with requested path
    char buffer[MAX_PATH_LEN];
    strncpy(buffer, root_dir, MAX_PATH_LEN);
    buffer[MAX_PATH_LEN - 1] = '\0';

    strcat(buffer, request->path);
    strncpy(request->path, buffer, MAX_PATH_LEN);
    request->path[MAX_PATH_LEN - 1] = '\0';

    // Check if path is escaping root dir and return result
    return is_escaping_path(request->path, root_dir);
}

void send_response_with_buffer(int client_socket, http_response_t* response) {
    char response_str[MAX_RESPONSE_LEN];
    set_http_header(response, response_str);
    ssize_t bytes_sent = send(client_socket, response_str, response->header_size, 0);
    if (bytes_sent < 0) {
        printf("[%s] Cannot send response header\n", DEBUG_ERR);
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Sent %ld bytes\n", DEBUG_INFO, bytes_sent);

    // set_body_response(response, "405 Method Not Allowed");
    ssize_t body_bytes_send = send(client_socket, response->body, response->body_size, 0);
}

void send_response_with_file(int client_socket, http_request_t request, http_response_t* response) {
    char response_str[MAX_RESPONSE_LEN];
    set_http_header(response, response_str);
    ssize_t bytes_send = send(client_socket, response_str, response->header_size, 0);
    if (bytes_send < 0) {
        printf("[%s] Cannot send response header\n", DEBUG_ERR);
        perror("send");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Sent %ld bytes\n", DEBUG_INFO, bytes_send);

    if (strcmp(request.method, HEAD_METHOD) == 0) {
        return;
    }

    int fd = open(request.path, O_RDONLY);
    if (fd == -1) {
        printf("[%s] Cannot open file %s\n", DEBUG_INFO, request.path);
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("[%s] File %s opened\n", DEBUG_INFO, request.path);

    off_t offset = 0;
    ssize_t bytes_sent = sendfile(client_socket, fd, &offset, response->body_size);
    if (bytes_sent < 0) {
        printf("[%s] Cannot send file %s\n", DEBUG_ERR, request.path);
        perror("send");
        exit(EXIT_FAILURE);
    }

    printf("Sent %ld bytes from file\n", bytes_sent);

    close(fd);
}

/*=========================================PUBLIC==============================================*/

void handle_request(int client_socket, char *root_dir) {
    // Receive request
    char request_str[MAX_REQUEST_LEN];
    ssize_t bytes_recieved = recv(client_socket, request_str, MAX_REQUEST_LEN - 1, 0);
    if (bytes_recieved < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    request_str[bytes_recieved] = '\0';
    printf("[%s] Recieved %ld bytes\n", DEBUG_INFO, bytes_recieved);

    printf("%s\n", request_str);

    // Declare request and response
    http_request_t request;
    http_response_t response;
    init_response(&response);

    int status = parse_request(request_str, &request, root_dir);
    if (status == 1) {
        printf("[%s] File %s not found\n", DEBUG_INFO, request.path);
        set_header_response(&response, HTTP_STATUS_NOT_FOUND, "text/html");
        set_body_response(&response, HTTP_NOT_FOUND_BODY);
        send_response_with_buffer(client_socket, &response);
        return;
    }
    if (status == -1) {
        return;
    }

    // Check if method is GET
    if (strcmp(request.method, GET_METHOD) != 0 && strcmp(request.method, HEAD_METHOD) != 0) {
        printf("[%s] Method not allowed\n", DEBUG_INFO);
        set_header_response(&response, HTTP_STATUS_METHOD_NOT_ALLOWED, "plain/text");
        set_body_response(&response, HTTP_NOT_ALLOWED_BODY);
        send_response_with_buffer(client_socket, &response);
        return;
    }
    printf("[%s] Method is GET or HEAD\n", DEBUG_INFO);

    // Determine whether path is a directory
    int is_index = 0;
    char path_buf[MAX_PATH_LEN];
    snprintf(path_buf, MAX_PATH_LEN, "%s", request.path);
    if (is_dir(path_buf)) {
        if (path_buf[strlen(path_buf) - 1] != '/') {
            strcat(path_buf, "/");
        }
        strcat(path_buf, INDEX_HTML);
        is_index = 1;
    }
    strncpy(request.path, path_buf, MAX_PATH_LEN);
    request.path[MAX_PATH_LEN - 1] = '\0';

    // Check if file exists
    if (access(request.path, F_OK) == -1) {
        printf("[%s] File %s not found\n", DEBUG_INFO, request.path);
        set_header_response(&response,
                           (is_index == 1) ? 
                                    HTTP_STATUS_FORBIDDEN : 
                                    HTTP_STATUS_NOT_FOUND, 
                            "text/html");
        set_body_response(&response, HTTP_NOT_FOUND_BODY);
        send_response_with_buffer(client_socket, &response);
        return;
    }
    printf("[%s] File %s was found\n", DEBUG_INFO, request.path);

    // Set response header
    set_header_response(&response, HTTP_STATUS_OK, get_content_type(request.path));

    // read the size of file
    struct stat st;
    if (stat(request.path, &st) == -1) {
        printf("[%s] Cannot get file stat\n", DEBUG_ERR);
        perror("stat");
        exit(EXIT_FAILURE);
    }
    response.body_size = st.st_size;
    printf("[%s] File size is %ld bytes\n", DEBUG_INFO, response.body_size);

    // Finally send response
    send_response_with_file(client_socket, request, &response);
}
