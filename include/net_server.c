
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bool.h"
#include "net.h"

void server_error(char *msg) {
    printf("Server Error: %s\n", msg);
    exit(1);
}

Server* create_server() {
    Server* server = (Server*)malloc(sizeof(Server));
    server->running = FALSE;

    return server;
}

void server_start(Server *server) {
    int child_fd, opt;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    fd_set read_fds;

    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket < 0) server_error("opening socket");

    opt = 1;
    setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server->port);

    if (bind(server->socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        server_error("binding");
    }

    if (listen(server->socket, MAX_CLIENT) < 0) server_error("listening");

    server->running = TRUE;
    while (server->running) {
        FD_ZERO(&read_fds);
        FD_SET(server->socket, &read_fds);
        FD_SET(0, &read_fds);

        if (select(server->socket + 1, &read_fds, 0, 0, 0) < 0) server_error("selecting");

        if (FD_ISSET(server->socket, &read_fds)) {
            child_fd = accept(server->socket, (struct sockaddr *)&client_addr, &client_len);
            if (child_fd < 0) server_error("accepting");
            server->handler(child_fd);
            close(child_fd);
        }
    }

    close(server->socket);
    free(server);
}

void server_stop(Server *server) {
    server->running = FALSE;
}
