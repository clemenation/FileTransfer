
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"
#include "net.h"
#include "protocol.h"

off_t fsize(const char *filename);

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
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