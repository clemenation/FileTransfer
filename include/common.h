#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <string.h>


#define TRUE 1
#define FALSE -1

void input(void *value, char *format, char *message) {
  char buffer[80];
  int len = 0;

  if (message != NULL) puts(message);

  fgets(buffer, sizeof(buffer), stdin);

  if (strncmp(format, "%s", 2) != 0) {
    sscanf(buffer, format, value);
  } else {
    // Overwrite newline with NULL terminate
    len = strlen(buffer);
    buffer[len - 1] = '\0';
    strncpy(value, buffer, len);
  }
}

void print_line(char delim, int len) {
  int i = 0;
  for (i = 0; i < len; i += 1) {
    printf("%c", delim);
  }
  printf("\n");
}

void die(char *msg) {
  perror(msg);
  exit(1);
}

#endif
