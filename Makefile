
LDFLAGS=-Linclude -lc -lssl -lcrypto
CFLAGS=-g -Wall -Iinclude

all: client server

client: client/client.o include/net_client.o include/net.o include/protocol.o include/checksum.o
	$(CC) $^ -o build/client/$@ $(LDFLAGS)

server: server/server.o include/net_server.o include/net.o include/protocol.o include/checksum.o
	$(CC) $^ -o build/server/$@ $(LDFLAGS)

clean:
	@rm -f */*.o
	@rm -rvf build/client/client
	@rm -rvf build/server
	@mkdir build/server

.PHONY:
