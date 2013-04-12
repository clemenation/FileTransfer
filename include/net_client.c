
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "bool.h"
#include "net.h"

void client_error(char *msg) {
    printf("Client Error: %s\n", msg);
    exit(1);
}

Client* create_client() {
    Client* client = (Client*)malloc(sizeof(Client));
    client->running = FALSE;

    return client;
}

void client_connect(Client *client, char *hostname) {
    struct sockaddr_in server_addr;
    struct hostent *server;

    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket < 0) client_error("opening socket");

    server = gethostbyname(hostname);
    if (server == NULL) client_error("no such host");

    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(client->port);

    if (connect(client->socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        client_error("connecting");
    }

    client->running = TRUE;
}

void client_disconnect(Client *client) {
    client->running = FALSE;
    close(client->socket);
    free(client);
}
