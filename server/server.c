
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
#include "checksum.h"

void handler(int client) {
  Message writeMsg;

  writeMsg = *receiveMessage(client, MSG_WRQ, "receive write request", &server_error);

  printf("MD5: ");
  printMD5Checksum(writeMsg.writeRequest.md5);
  printf("\n");

  int outputFile = open(writeMsg.writeRequest.fileName, 
    O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
  if (outputFile == -1)
  {
    server_error("cannot open output file");
  }

  int id = 0;
  while (1)
  {
    Message *dataMsg;

    dataMsg = receiveMessage(client, MSG_DAT, "receive data packet", &server_error);
    if (!dataMsg)
    { // we're done receiving the file
      break;
    }

    int bytes_written = Write(outputFile, 
      dataMsg->dataPacket.data, 
      dataMsg->dataPacket.size);
    id = dataMsg->dataPacket.id;
    if (bytes_written <= 0)
    { // handle errors
      server_error("write to output file");
    }

    free(dataMsg);  // free after use
  }

  close(outputFile);
  printf("Finished receiving %s\n", writeMsg.writeRequest.fileName);
  printf("%d bytes received\n", writeMsg.writeRequest.fileSize);
  printf("%d packet received\n", id+1);  

  // md5 matching
  unsigned char *md5 = MD5Checksum(writeMsg.writeRequest.fileName);
  if (memcmp(md5, writeMsg.writeRequest.md5, MD5_DIGEST_LENGTH) == 0)
  {
    printf("MD5 match\n");
  }
  else
  {
    server_error("MD5 mismatch");
  }
  free(md5);
}

int main(int argc, char *argv[]) {
    Server *server = create_server();

    server->port = PORT;
    server->handler = &handler;
    server_start(server);

    return 0;
}
