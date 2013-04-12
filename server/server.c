
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

void handler(int client) {
  int n;
  char buff[BUFFER_SIZE];

  bzero(buff, BUFFER_SIZE);
  n = read(client, buff, BUFFER_SIZE);
  if (n < 0) server_error("reading to socket");

  printf("Server received %s\n", buff);

  n = write(client, buff, BUFFER_SIZE);
  if (n < 0) server_error("writing to socket");
}

int main() {
    Server *server = create_server();

    server->port = PORT;
    server->handler = &handler;
    server_start(server);

    return 0;
}
