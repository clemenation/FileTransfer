
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "net.h"
#include "protocol.h"
#include "checksum.h"

void handler(char *msg) {
    printf("Echo from server: %s\n", msg);
}

void client_send(Client *client, char *filename) {
    int inputFile;  // input file descriptor
    char buff[BUFFER_SIZE]; // buffer

    Message writeMsg = sendWriteMessage(client->socket, filename, &client_error);

    Message ackMsg = *receiveMessage(client->socket, MSG_ACK, "receive ack message", &client_error);
    if (!ackMsg.ackowledgementPacket.id)
    {
        client_error("server denied");
    }

    printf("Accept message received\n");

    // open the file
    inputFile = open(filename, O_RDONLY);
    if (inputFile == -1)
    {   // handle errors
        client_error("cannot open input file");
    }
    printf("MD5: ");
    printMD5Checksum(writeMsg.writeRequest.md5);
    printf("\n");

    int id = 0;
    int bytes_read;
    while (1) 
    {
        // Read data into buffer.  We may not have enough to fill up buffer, so we
        // store how many bytes were actually read in bytes_read.
        bytes_read = Read(inputFile, buff, sizeof(buff));

        if (bytes_read == 0)
        {   // end of file
            break;
        }

        if (bytes_read < 0) {
            // handle errors
            client_error("input file reading");
        }

        sendDataMessage(client->socket, id, buff, bytes_read, &client_error);

        id++;   // increment id counting
    }

    close(inputFile);
    printf("Finished sending %s\n", filename);
    printf("%d bytes sent\n", writeMsg.writeRequest.fileSize);
    printf("%d packet sent\n", id);
}

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        printf("Usage: %s [hostname] [filename]\n", argv[0]);
        exit(1);
    }

    FILE *inputFile = fopen(argv[2], "rb");
    if (inputFile == NULL)
    {
        client_error("cannot open file");
    }
    fclose(inputFile);

    Client *client = create_client();
    client->port = PORT;
    client->handler = &handler;
    client_connect(client, argv[1]);

    client_send(client, argv[2]);

    client_disconnect(client);

    return 0;
}
