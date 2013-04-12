
LDFLAGS=-Linclude -lc
CFLAGS=-g -Wall -Iinclude

all: client server

client: client/client.o include/net_client.o
	$(CC) $(LDFLAGS) $^ -o build/client/$@

server: server/server.o include/net_server.o
	$(CC) $(LDFLAGS) $^ -o build/server/$@

clean:
	@rm -f */*.o
	@rm -rvf build/client/client
	@rm -rvf build/server/server

.PHONY:
