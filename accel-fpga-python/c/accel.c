#include <stdint.h>
#include <string.h>

#include "accel.h"


void calculate_512_bit_chunk(uint8_t *buff_512_bit, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e) {
   int i,ii;
   uint32_t w[80];

   uint32_t f, k;
   uint32_t temp;

   ii = 0;
   for(i=0; i<=15; i++) {
      #pragma HLS UNROLL
      ((uint8_t*) &w[i])[0] = buff_512_bit[ii + 3];
      ((uint8_t*) &w[i])[1] = buff_512_bit[ii + 2];
      ((uint8_t*) &w[i])[2] = buff_512_bit[ii + 1];
      ((uint8_t*) &w[i])[3] = buff_512_bit[ii + 0];

      ii+=4;
   }

   for(i=16; i<80; i++) {
      #pragma HLS UNROLL
      w[i] = w[i-3]^w[i-8]^w[i-14]^w[i-16];
      w[i] = ROL32(w[i], 1);
   }

   for(i=0; i<80; i++) {
      if(i < 20) {
         f = (*b & *c) | (~*b & *d);
         k = 0x5A827999;

      } else if( i < 40) {
         f = *b^*c^*d;
         k = 0x6ED9EBA1;

      } else if(i < 60) {
         f = (*b & *c) | (*b & *d) | (*c & *d);
         k = 0x8F1BBCDC;

      } else {
         f = *b^*c^*d;
         k = 0xCA62C1D6;
      }

      temp = ROL32(*a, 5) + f + *e + k + w[i];

      *e = *d;
      *d = *c;
      *c = ROL32(*b, 30);
      *b = *a;
      *a = temp;
   }
}


void append_message_len(uint8_t* buff_512_bit, uint32_t *message_len) {
   uint64_t message_bit_len = *message_len * 8;
   uint8_t i;

   for(i=0; i<7; i++)
      buff_512_bit[63 - i] = ((uint8_t*) &message_bit_len)[i];
}


void sha1(uint8_t *message, uint32_t *message_len, uint8_t* hash_160_bit) {
   #pragma HLS INTERFACE m_axi port=message offset=slave
   #pragma HLS INTERFACE m_axi port=message_len depth=1 offset=slave
   #pragma HLS INTERFACE m_axi port=hash_160_bit depth=20 offset=slave
   #pragma HLS INTERFACE s_axilite port=return

   uint8_t i;

   uint32_t h0 = 0x67452301;
   uint32_t h1 = 0xEFCDAB89;
   uint32_t h2 = 0x98BADCFE;
   uint32_t h3 = 0x10325476;
   uint32_t h4 = 0xC3D2E1F0;

   uint32_t a, b, c, d, e;

   uint8_t buff_512_bit[64] = {0};

   uint8_t last_chunk_len;
   uint32_t total_chunks;
   uint32_t chunk;

   total_chunks = *message_len / 64;
   last_chunk_len = *message_len % 64;

   for(chunk=0; chunk<total_chunks; chunk++) {
      a = h0;
      b = h1;
      c = h2;
      d = h3;
      e = h4;

      calculate_512_bit_chunk((uint8_t *) message+(chunk * 64), &a, &b, &c, &d, &e);

      h0 = h0 + a;
      h1 = h1 + b;
      h2 = h2 + c;
      h3 = h3 + d;
      h4 = h4 + e;
   }

   memcpy(buff_512_bit, (message+(total_chunks * 64)), last_chunk_len);

   buff_512_bit[last_chunk_len] |= 0x80;

   if (last_chunk_len < 56)
      append_message_len(buff_512_bit, message_len);

   a = h0;
   b = h1;
   c = h2;
   d = h3;
   e = h4;

   calculate_512_bit_chunk(buff_512_bit, &a, &b, &c, &d, &e);

   h0 = h0 + a;
   h1 = h1 + b;
   h2 = h2 + c;
   h3 = h3 + d;
   h4 = h4 + e;

   if (last_chunk_len > 55) {
      memset(buff_512_bit, 0, 64);

      append_message_len(buff_512_bit, message_len);

      a = h0;
      b = h1;
      c = h2;
      d = h3;
      e = h4;

      calculate_512_bit_chunk(buff_512_bit, &a, &b, &c, &d, &e);

      h0 = h0 + a;
      h1 = h1 + b;
      h2 = h2 + c;
      h3 = h3 + d;
      h4 = h4 + e;
   }


   for(i=0; i<4; i++)
      hash_160_bit[i] = ((uint8_t*) &h0)[3 - i];


   for(i=0; i<4; i++)
      hash_160_bit[i + 4] = ((uint8_t*) &h1)[3 - i];


   for(i=0; i<4; i++)
      hash_160_bit[i + 8] = ((uint8_t*) &h2)[3 - i];


   for(i=0; i<4; i++)
      hash_160_bit[i + 12] = ((uint8_t*) &h3)[3 - i];


   for(i=0; i<4; i++)
      hash_160_bit[i + 16] = ((uint8_t*) &h4)[3 - i];
}

