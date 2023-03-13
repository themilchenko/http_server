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
#include <ctype.h>
#include <time.h>

#include "server.h"
#include "config.h"

void get_cwd(char *cwd) {
    getcwd(cwd, MAX_PATH_LEN);
    cwd[MAX_PATH_LEN - 1] = '\0';
}

int main() {
    char cwd[MAX_PATH_LEN];
    get_cwd(cwd);
    printf("%s\n", cwd);

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
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("[%s] Socket bind failed\n", DEBUG_ERR);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Socket binded\n", DEBUG_INFO);

    if (listen(server_socket, BACKLOG) < 0) {
        printf("[%s] Socket listen failed\n", DEBUG_ERR);
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Server listening %d port...\n", DEBUG_INFO, PORT);
    
    int cpu_count = 0;
    int status;
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            printf("[%s] Socket accept failed\n", DEBUG_ERR);
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("[%s] Accepted connection on socket from %s:%d\n", DEBUG_INFO, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // // For testing purposes
        // handle_request(client_socket);
        // close(client_socket);

        // Fork a child process to handle the request
        pid_t pid = fork();
        if (pid < 0) {
            printf("[%s] Fork failed\n", DEBUG_ERR);
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(server_socket);
            handle_request(client_socket);
            close(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);
            wait(&status); // Wait for child process to finish
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
                --cpu_count;
            }
        }

        // Limit the number of child processes
        if (cpu_count >= CPU_LIMIT) {
            printf("[%s] CPU limit reached, waiting for child processes to finish\n", DEBUG_INFO);
            wait(NULL);
            --cpu_count;
        }

        ++cpu_count;
    }

    return EXIT_SUCCESS;
}
