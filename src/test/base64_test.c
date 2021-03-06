// REMAKE
#if 0

#include "../log.h"
#include "../crypto/base64.h"

static const char plain_msg[] = "a very long message just to be sure!";
bool base64_test(void){
	char msg[256];
	char encoded[256];
	size_t len;

	strcpy(msg, plain_msg);
	len = strlen(msg);
	LOG("message: %s", msg);
	base64_encode(encoded, (const uint8*)msg, len);
	LOG("encoded: %s", encoded);
	base64_decode((uint8*)msg, len, encoded);
	LOG("decoded: %s", msg);
	getchar();
	return false;
}

#endif
