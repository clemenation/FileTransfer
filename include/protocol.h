
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define PORT 5000
#define BUFFER_SIZE 512

typedef enum MessageType_ 
{
  MSG_WRQ, MSG_DAT, MSG_ACK, MSG_TXT
} MessageType;

typedef struct WriteRequestMessage_
{
  char fileName[BUFFER_SIZE];
  int fileSize;
} WriteRequestMessage;

typedef struct DataPacketMessage_
{
  int id;
  char data[BUFFER_SIZE];
  int size;
} DataPacketMessage;

typedef struct AcknowledgementPacketMessage_
{
  int id;
} AcknowledgementPacketMessage;

typedef struct TextMessage_
{
  char content[BUFFER_SIZE];
} TextMessage;

typedef struct Message_
{
  MessageType type;
  union
  {
    WriteRequestMessage writeRequest;
    DataPacketMessage dataPacket;
    AcknowledgementPacketMessage ackowledgementPacket;
    TextMessage text;
  };
} Message;

#endif
