#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "accel.h"

#define MESSAGE_LEN (10485760 * 3)
#define MESSAGE_CHAR 0

int main() {
   uint8_t i;

   uint8_t hash[20] = {0};

   uint8_t *message = NULL;
   uint32_t message_len;

   message = malloc(MESSAGE_LEN * sizeof(uint8_t));
   memset(message, MESSAGE_CHAR, MESSAGE_LEN);
   message_len = MESSAGE_LEN;

   sha1(message, &message_len, hash);
      
   printf("Hash: ");
   for(i=0; i<20; i++)
     printf("%02x", hash[i]);
   printf("\n");

   free(message);
  
   return 0;
}
