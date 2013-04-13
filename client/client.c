
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

void handler(char *msg) {
    printf("Echo from server: %s\n", msg);
}

void client_send(Client *client, char *filename) {
    int inputFile;  // input file descriptor
    char buff[BUFFER_SIZE]; // buffer
    int bytes_written = 0;  // counting bytes written to socket

    // Create write request
    Message writeMsg = writeMessage(filename);    

    // Send write request
    bytes_written = Write(client->socket, &writeMsg, sizeof(writeMsg));

    inputFile = open(filename, O_RDONLY);
    if (inputFile == -1)
    {   // handle errors
        client_error("cannot open input file");
    }

    int id = 0;
    while (1) {
        // Read data into buffer.  We may not have enough to fill up buffer, so we
        // store how many bytes were actually read in bytes_read.
        int bytes_read = read(inputFile, buff, sizeof(buff));

        if (bytes_read == 0) // We're done reading from the file
            break;

        if (bytes_read < 0) {
            // handle errors
            client_error("input file reading");
        }

        Message dataMsg = dataMessage(id, buff, bytes_read);
        bytes_written = Write(client->socket, &dataMsg, sizeof(dataMsg));

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

    Client *client = create_client();
    client->port = PORT;
    client->handler = &handler;
    client_connect(client, argv[1]);

    client_send(client, argv[2]);

    client_disconnect(client);

    return 0;
}
