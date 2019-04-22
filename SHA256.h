#ifndef SHA256_H
#define SHA256_H

#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA256_BLOCK_SIZE  ( 512 / 8)

// C99 defines stdint.h which defines a uint32_t and uint8_t type.
// But the other kids didn't want to play with poor little Solaris 9 and so he
// just defines these in inttypes.h which is also part of C99 and is supposed to
// include stdint.h. Solaris 9 is a weirdo. :(
#include <inttypes.h>

typedef struct {
  unsigned int tot_len;
  unsigned int len;
  unsigned char block[2 * SHA256_BLOCK_SIZE];
  uint32_t h[8];
} sha256_ctx;

void sha256_init(sha256_ctx * ctx);
void sha256_update(sha256_ctx *ctx, const unsigned char *message,
                   unsigned int len);
void sha256_final(sha256_ctx *ctx, unsigned char *digest);
void sha256(const unsigned char *message, unsigned int len,
            unsigned char *digest);
#endif // !SHA256_H