
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

off_t fsize(const char *filename);

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

Message ackMessage(int id)
{
  Message msg;
  msg.type = MSG_ACK;
  
  AcknowledgementPacketMessage payload;
  payload.id = id;

  msg.ackowledgementPacket = payload;

  return msg;
}

Message writeMessage(char *filename)
{
    Message msg;
    msg.type = MSG_WRQ;

    WriteRequestMessage payload;

    printf("Getting MD5 checksum\n");
    unsigned char *md5 = MD5Checksum(filename);
    memcpy(payload.md5, md5, MD5_DIGEST_LENGTH);
    free(md5);

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
    memcpy(payload.data, data, size);
    msg.dataPacket = payload;

    return msg;
}

#pragma mark - Sending
Message sendMessage(int socket, 
    Message message, 
    char *errorMessage,
    void (*errorFunction)(char *))
{
    if (Write(socket, &message, sizeof(message)) < 0)
    {
        errorFunction(errorMessage);
    }
    return message;
}

Message sendWriteMessage(int socket, 
    char *filename,
    void (*errorFunction)(char *))
{
  FILE *inFile = fopen (filename, "rb");
  if (inFile == NULL)
  {
    errorFunction("cannot open file");
  }
  fclose(inFile);

  return sendMessage(socket, 
      writeMessage(filename), 
      "cannot send write request", 
      errorFunction);
}

Message sendDataMessage(int socket, 
    int id, 
    char *data, 
    int size,
    void (*errorFunction)(char *))
{
    return sendMessage(socket, 
        dataMessage(id, data, size), 
        "cannot send data packet", 
        errorFunction);
}

Message sendAckMessage(int socket,
  int id,
  void (*errorFunction)(char *))
{
  return sendMessage(socket, 
    ackMessage(id),
    "cannot send acknowledgement packet",
    errorFunction);
}

#pragma mark - Receiving
Message *receiveMessage(int socket, 
  MessageType messageType, 
  char *errorMessage, 
  void (*errorFunction)(char *))
{
  Message *message = (Message *)malloc(sizeof(Message));
  int bytes_read = Read(socket, message, sizeof(*message));
  if (bytes_read < 0)
  { // reading error
    errorFunction(errorMessage);
  }
  else if (bytes_read == 0)
  { // no message, return NULL
    return NULL;
  }
  if (message->type != messageType)
  { // wrong message type error
    errorFunction("incompatible message type");
  }

  return message;
}
