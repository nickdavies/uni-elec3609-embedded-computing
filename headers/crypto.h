#ifndef crypto_GUARD
#define crypto_GUARD

#include <stdint.h>


#define CRYPTO_ENABLE_ENCRYPTION 1

/* should be quite long so that its definatly longer than the datagram */
#define CRYPTO_PASSWORD (uint8_t*)"lUBe43hOnEzin50IdeAl"

uint8_t encrypt(uint8_t * pw, uint8_t * data,uint32_t len);
uint8_t decrypt(uint8_t * pw, uint8_t * data,uint32_t len);

#endif
