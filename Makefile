
LDFLAGS=-Linclude -lc
CFLAGS=-g -Wall -Iinclude

all: client server

client: client/client.o include/net_client.o
	$(CC) $(LDFLAGS) $^ -o build/$@

server: server/server.o include/net_server.o
	$(CC) $(LDFLAGS) $^ -o build/$@

clean:
	@rm -rvf build
	@mkdir build

.PHONY:
