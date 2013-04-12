
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

void handler(char *msg) {
    printf("Echo from server: %s\n", msg);
}

void client_send(Client *client, char *filename) {
    int inputFile;
    char buff[BUFFER_SIZE];

    inputFile = open(filename, O_RDONLY);
    if (inputFile == -1)
    {
        client_error("cannot open input file");
    }

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

        // You need a loop for the write, because not all of the data may be written
        // in one call; write will return how many bytes were written. p keeps
        // track of where in the buffer we are, while we decrement bytes_read
        // to keep track of how many bytes are left to write.
        void *p = buff;
        while (bytes_read > 0) {
            int bytes_written = write(client->socket, p, bytes_read);
            if (bytes_written <= 0) {
                // handle errors
                client_error("write to socket");
            }
            bytes_read -= bytes_written;
            p += bytes_written;
        }
    }

    close(inputFile);
    printf("Finished sending\n");
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
