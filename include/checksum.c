#include <stdio.h>
#include <stdlib.h>
#include "checksum.h"

#if defined( __APPLE__ )
  // fix deprecated message in OS X >= 10.7
    #include <CommonCrypto/CommonDigest.h>

    #ifdef MD5_DIGEST_LENGTH

        #undef MD5_DIGEST_LENGTH

    #endif

    #define MD5_Init            CC_MD5_Init
    #define MD5_Update          CC_MD5_Update
    #define MD5_Final           CC_MD5_Final
    #define MD5_DIGEST_LENGTH   CC_MD5_DIGEST_LENGTH
    #define MD5_CTX             CC_MD5_CTX

#else

    #include <openssl/md5.h>

#endif

unsigned char *MD5Checksum(char *filename)
{
  unsigned char *c = (unsigned char *)malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
  FILE *inFile = fopen (filename, "rb");
  if (inFile == NULL)
  {
    printf("Error: Cannot open file %s\n", filename);
    exit(1);
  }
  MD5_CTX mdContext;
  int bytes;
  unsigned char data[1024];

  if (inFile == NULL) {
      return NULL;
  }

  MD5_Init (&mdContext);
  while ((bytes = fread (data, 1, 1024, inFile)) != 0)
      MD5_Update (&mdContext, data, bytes);
  MD5_Final (c,&mdContext);
  fclose (inFile);
  return c;
}

void printMD5Checksum(unsigned char *c)
{
  int i;
  for(i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]);
}