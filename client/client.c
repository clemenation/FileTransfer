
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

void handler(char *msg) {
    printf("Echo from server: %s\n", msg);
}

void client_send(Client *client, char *m) {
    int n;
    char buff[BUFFER_SIZE];

    n = write(client->socket, m, strlen(m));
    if (n < 0) client_error("writing to socket");

    bzero(buff, BUFFER_SIZE);
    n = read(client->socket, buff, BUFFER_SIZE);
    if (n < 0) client_error("reading to socket");

    client->handler(buff);
}

int main() {
    Client *client = create_client();
    client->port = PORT;
    client->handler = &handler;
    client_connect(client, "localhost");

    char buff[BUFFER_SIZE];
    printf("Please enter msg: ");
    bzero(buff, BUFFER_SIZE);
    fgets(buff, BUFFER_SIZE, stdin);
    client_send(client, buff);

    return 0;
}
