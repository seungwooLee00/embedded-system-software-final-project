#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

#define ENCRYPTION(plain, cipher) syscall(379, plain, cipher, 0)
#define DECRYPTION(plain, cipher) syscall(379, plain, cipher, 1)

int main (void) {
  char original[] = "embeddedsoftware";
  char cipher[17] = {0};
  char plain[17] = {0};

  ENCRYPTION(original, cipher);
  DECRYPTION(plain, cipher);

  printf ("%s\n", cipher);
  printf ("%s\n", plain);
  
}



