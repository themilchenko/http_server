#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#include "server.h"
#include "openssl.h"
#include "config.h"

#define CPU_LIMIT 4

int main(int argc, char *argv[]) {
    cfg_t *cfg = malloc(sizeof(cfg_t));
    init_config(cfg);

    if (argc < 1 || argc > 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        if (access(argv[1], R_OK) != 0) {
            perror("file access");
            exit(EXIT_FAILURE);
        }
        if (parse_config(argv[1], cfg) == -1) {
            printf("File %s doesn't exist\n", argv[1]);
        }
    } else {
        if (init_config(cfg) == -1) {
            printf("Error while initializing config\n");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize OpenSSL
    SSL_CTX *ctx = NULL;
    if (access(cfg->ssl_cert, R_OK) == 0 && access(cfg->ssl_key, R_OK) == 0) {
        printf("SSL files found, enabling HTTPS\n");
        init_openssl();

        ctx = create_ssl_context(cfg);
    } else {
        printf("SSL files not found, using HTTP\n");
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("[%s] Socket creation failed\n", DEBUG_ERR);
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("[%s] Socket created\n", DEBUG_INFO);

    // Enabling address reuse
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        printf("[%s] Socket setsockopt failed\n", DEBUG_ERR);
        perror("setsockopt");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(cfg->port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("[%s] Socket bind failed\n", DEBUG_ERR);
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[%s] Socket binded\n", DEBUG_INFO);

    if (listen(server_socket, BACKLOG) < 0) {
        printf("[%s] Socket listen failed\n", DEBUG_ERR);
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[%s] Server listening %d port...\n", DEBUG_INFO, cfg->port);
    
    int cpu_count = 0;
    int status;
    pid_t child_pid;
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            printf("[%s] Socket accept failed\n", DEBUG_ERR);
            perror("accept");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("[%s] Accepted connection on socket from %s:%d\n", DEBUG_INFO, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Fork a child process to handle the request
        pid_t pid = fork();
        printf("CPU count: %d\n", cpu_count);
        if (pid < 0) {
            printf("[%s] Fork failed\n", DEBUG_ERR);
            perror("fork");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {
            // Child process
            SSL *ssl = NULL;
            if (ctx) {
                ssl = SSL_new(ctx);
                SSL_set_fd(ssl, client_socket);
                if (SSL_accept(ssl) <= 0) {
                    printf("[%s] SSL handshake failed\n", DEBUG_ERR);
                    ERR_print_errors_fp(stderr);
                    close(client_socket);
                    continue;
                }
                printf("[%s] SSL handshake successful\n", DEBUG_INFO);
            }

            close(server_socket);

            handle_request(client_socket, ssl, cfg->document_root);
            if (ssl) {
                SSL_free(ssl);
            }

            close(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);

            ++cpu_count; // Only increment if a child process was successfully created

            if (cpu_count >= CPU_LIMIT) {
                printf("[%s] CPU limit reached, waiting for child processes to finish...\n", DEBUG_INFO);

                child_pid = waitpid(-1, &status, 0);
                while (child_pid > 0) {
                    --cpu_count;
                    child_pid = waitpid(-1, &status, 0);
                }
            }
        }
    }

    free(cfg);
    cleanup_openssl();
    return EXIT_SUCCESS;
}
