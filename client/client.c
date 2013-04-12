
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

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
            client_error("write to socket");
        }
        bytes_towrite -= bytes_written;
        p += bytes_written;
    }

    return (numbytes - bytes_towrite);
}

Message writeMessage(char *filename)
{
    Message msg;
    msg.type = MSG_WRQ;

    WriteRequestMessage payload;
    strcpy(payload.fileName, filename);
    payload.fileSize = fsize(filename);
    msg.writeRequest = payload;

    return msg;
}

Message dataMessage(int id, char *data, int size)
{
    Message msg;
    msg.type = MSG_DAT;

    DataPacketMessage payload;
    payload.id = id;
    payload.size = size;
    strncpy(payload.data, data, size);
    msg.dataPacket = payload;

    return msg;
}

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
