
#ifndef _COMMON_NET_
#define _COMMON_NET_

#define MAX_CLIENT 32

typedef struct Server_ {
  int socket;
  int running;
  int port;
  void (*handler)(int);
} Server;

typedef struct Client_ {
  int socket;
  int running;
  int port;
  void (*handler)(char*);
} Client;

Server *create_server();
void server_start(Server *server);
void server_stop(Server *server);
void server_error(char *msg);

Client *create_client();
void client_connect(Client *client, char *hostname);
void client_disconnect(Client *client);
void client_error(char *msg);

#endif
