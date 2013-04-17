
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define PORT 5000
#define BUFFER_SIZE 512
#define MD5_DIGEST_LENGTH 16

typedef enum MessageType_ 
{
  MSG_WRQ, MSG_DAT, MSG_ACK, MSG_TXT
} MessageType;

typedef struct WriteRequestMessage_
{
  char fileName[BUFFER_SIZE];
  int fileSize;
  unsigned char md5[MD5_DIGEST_LENGTH];
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

Message writeMessage(char *filename);
Message dataMessage(int id, char *data, int size);
Message ackMessage(int id);

#pragma mark - Sending
Message sendMessage(int socket, 
    Message message, 
    char *errorMessage,
    void (*errorFunction)(char *));
Message sendWriteMessage(int socket, 
    char *filename,
    void (*errorFunction)(char *));
Message sendDataMessage(int socket, 
    int id, 
    char *data, 
    int size,
    void (*errorFunction)(char *));
Message sendAckMessage(int socket,
  int id,
  void (*errorFunction)(char *));

#pragma mark - Receiving
Message *receiveMessage(int socket, 
  MessageType messageType, 
  char *errorMessage, 
  void (*errorFunction)(char *));

#endif
