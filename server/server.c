
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

int Write(int fd, void * ptr, int numbytes)
{
    // You need a loop for the write, because not all of the data may be written
    // in one call; write will return how many bytes were written. p keeps
    // track of where in the buffer we are, while we decrement bytes_towrite
    // to keep track of how many bytes are left to write.
    void *p = ptr;
    int bytes_towrite = numbytes;
    while (bytes_towrite > 0) {
        int bytes_written = write(fd, p, bytes_towrite);
        if (bytes_written <= 0) {
            // handle errors
            server_error("write to socket");
        }
        bytes_towrite -= bytes_written;
        p += bytes_written;
    }

    return (numbytes - bytes_towrite);
}

void handler(int client) {
  int bytes_read = 0;
  Message writeMsg;

  bytes_read = read(client, &writeMsg, sizeof(writeMsg));

  if (writeMsg.type == MSG_WRQ)
  {
    int outputFile;

    outputFile = open(writeMsg.writeRequest.fileName, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (outputFile == -1)
    {
      server_error("cannot open output file");
    }

    int id = 0;
    while (1)
    {
      Message dataMsg;

      bytes_read = read(client, &dataMsg, sizeof(dataMsg));
      if (bytes_read < 0)
      { // handle errors
        server_error("read from socket");
      }

      if (bytes_read == 0)
      { // we're done receiving the file
        break;
      }

      int bytes_written = write(outputFile, dataMsg.dataPacket.data, dataMsg.dataPacket.size);
      id = dataMsg.dataPacket.id;
      if (bytes_written <= 0)
      { // handle errors
        server_error("write to output file");
      }
    }

    close(outputFile);
    printf("Finished receiving %s\n", writeMsg.writeRequest.fileName);
    printf("%d bytes received\n", writeMsg.writeRequest.fileSize);
    printf("%d packet received\n", id+1);
  }
}

int main(int argc, char *argv[]) {
    Server *server = create_server();

    server->port = PORT;
    server->handler = &handler;
    server_start(server);

    return 0;
}
