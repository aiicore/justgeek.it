#include <stdint.h>

#define ROL32(x, n) ((x << n) | (x >> (32 - n)))

void calculate_512_bit_chunk(uint8_t *buff_512_bit, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e);

void append_message_len(uint8_t* buff_512_bit, uint32_t *message_len);

void sha1(uint8_t *message, uint32_t *message_len, uint8_t* hash_160_bit);

