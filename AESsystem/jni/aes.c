#include "myJni.h"

#define ENCRYPTION(plain, cipher) syscall(379, plain, cipher, 0)
#define DECRYPTION(plain, cipher) syscall(379, plain, cipher, 1)

char*
doCipher (char *buf) {
  char *plain = buf, *cipher;
  LOGV("length of plaintext: %d", strlen(plain));
  cipher = malloc (sizeof(char) * 17);
  LOGV("before encrypt: %s", plain);
  ENCRYPTION(plain, cipher);
  LOGV("after encrypt: %s", cipher);
  cipher[16] = '\0';

  return cipher;
}

char*
doReverseCipher (char *buf) {
  char *plain, *cipher = buf;
  LOGV("length of ciphertext: %d", strlen(cipher));
  plain = malloc (sizeof(char) * 17);
  LOGV("before decrypt: %s", cipher);
  DECRYPTION(plain, buf);
  plain[16] = '\0';
  LOGV("after decrypt: %s", plain);
  return plain;
}