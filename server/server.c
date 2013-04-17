
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "config.h"
#include "net.h"
#include "protocol.h"
#include "checksum.h"

void receiveFile(int client, Message writeMsg);
void handler(int client);

int main(int argc, char *argv[]) {
    Server *server = create_server();

    server->port = PORT;
    server->handler = &handler;
    server_start(server);

    return 0;
}

void receiveFile(int client, Message writeMsg)
{
  int outputFile = open(writeMsg.writeRequest.fileName, 
  O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
  if (outputFile == -1)
  {
    sendAckMessage(client, 0, &server_error);  // 0 means deny, else means accept
    server_error("cannot open output file");
  }
  sendAckMessage(client, 1, &server_error);  // accept

  int id = 0;
  double ttime = (double)clock()/CLOCKS_PER_SEC;
  int prevId = 0;
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

    if ((double)clock()/CLOCKS_PER_SEC - ttime >= UPDATE_TIME)
    {
      ttime = (double)clock()/CLOCKS_PER_SEC;
      int received = id*BUFFER_SIZE + dataMsg->dataPacket.size;
      int total = writeMsg.writeRequest.fileSize;
      printf("Received %.1f/%.1f KB | %.0f%% completed | %.1f KB/s\n",
        received / 1024.0, 
        total / 1024.0,
        (float)received/(float)total*100.0,
        (float)(id-prevId) * BUFFER_SIZE / 1024.0);
      prevId = id;
    }

    free(dataMsg);  // free after use
  }

  close(outputFile);
  printf("Finished receiving %s\n", writeMsg.writeRequest.fileName);
  printf("%d bytes received\n", writeMsg.writeRequest.fileSize);
  printf("%d packet received\n", id+1);  

  // md5 matching
  printf("Getting MD5 checksum\n");
  unsigned char *md5 = MD5Checksum(writeMsg.writeRequest.fileName);
  if (memcmp(md5, writeMsg.writeRequest.md5, MD5_DIGEST_LENGTH) == 0)
  {
    printf("MD5 match");
  }
  else
  {
    printf("ERROR: MD5 mismatch");
  }
  printf("(");
  printMD5Checksum(md5);
  printf(")\n");
  free(md5);
}

void handler(int client) {
  Message writeMsg;

  writeMsg = *receiveMessage(client, MSG_WRQ, "receive write request", &server_error);

  printf("\n========================================\n\n");
  printf("Received write request of file %s\n", writeMsg.writeRequest.fileName);
  printf("File size: %d bytes\n", writeMsg.writeRequest.fileSize);
  printf("MD5: ");
  printMD5Checksum(writeMsg.writeRequest.md5);
  printf("\n");

  char choice;
  printf("Accept?(y\\n): "); scanf("%c", &choice); getchar();

  if (choice == 'y')
  {
    receiveFile(client, writeMsg);
  }
  else
  {
    sendAckMessage(client, 0, &server_error);   // deny
    printf("Deny message sent\n");
  }
}