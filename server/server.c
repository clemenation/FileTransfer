
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

void handler(int client) {
  int outputFile;
  char buff[BUFFER_SIZE];

  outputFile = open("output.txt", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
  if (outputFile == -1)
  {
    server_error("cannot open output file");
  }

  while (1)
  {
    int bytes_read = read(client, buff, sizeof(buff));
    if (bytes_read < 0)
    { // handle errors
      server_error("read from socket");
    }

    if (bytes_read == 0)
    { // we're done receiving the file
      break;
    }

    int bytes_written = write(outputFile, buff, bytes_read);
    if (bytes_written <= 0)
    { // handle errors
      server_error("write to output file");
    }
  }

  close(outputFile);
  printf("Finished receiving");
}

int main(int argc, char *argv[]) {
    Server *server = create_server();

    server->port = PORT;
    server->handler = &handler;
    server_start(server);

    return 0;
}
