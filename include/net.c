#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "bool.h"
#include "net.h"

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
      if (bytes_written < 0) {
          // handle errors
          return bytes_written;
      } else if (bytes_written == 0) break;
      bytes_towrite -= bytes_written;
      p += bytes_written;
  }

  return (numbytes - bytes_towrite);
}

int Read(int fd, void * ptr, int numbytes)
{
  // You need a loop for the read, because not all of the data may be read
  // in one call; read will return how many bytes were read. p keeps
  // track of where in the buffer we are, while we decrement bytes_toread
  // to keep track of how many bytes are left to read.
  void *p = ptr;
  int bytes_toread = numbytes;
  while (bytes_toread > 0) {
      int bytes_read = read(fd, p, bytes_toread);
      if (bytes_read < 0) {
          // handle errors
          return bytes_read;
      } else if (bytes_read == 0) break;
      bytes_toread -= bytes_read;
      p += bytes_read;
  }

  return (numbytes - bytes_toread);
}