#ifndef CRYPTO_ARC4RANDOM_H_
#define CRYPTO_ARC4RANDOM_H_

#include "../def.h"

#ifdef PLATFORM_BSD
#include <stdlib.h>
#else
void arc4random_buf(void *data, size_t len);
uint32 arc4random(void);
#endif

#endif //CRYPTO_ARC4RANDOM_H_
