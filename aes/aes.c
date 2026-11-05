#include "aes.h"

static uint8_t
reduce (uint8_t x) {
  return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

static uint8_t
multiply (uint8_t x, uint8_t y) {
  uint8_t result = 0;
  uint8_t temp = x;

  while (y) {
    if (y & 1) {
      result ^= temp;
    }
    temp = reduce (temp);
    y >>= 1;
  }

  return result;
}

void
subBytes (uint8_t state[4][4]) {
  int i, j;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[i][j] = sbox[state[i][j]];
    }
  }
}

void
invSubBytes (uint8_t state[4][4]) {
  int i, j;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[i][j] = invSbox[state[i][j]];
    }
  }
}

void
shiftRows (uint8_t state[4][4]) {
  uint8_t temp;

  temp = state[1][0];
  state[1][0] = state[1][1];
  state[1][1] = state[1][2];
  state[1][2] = state[1][3];
  state[1][3] = temp;

  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;

  temp = state[3][0];
  state[3][0] = state[3][1];
  state[3][1] = state[3][2];
  state[3][2] = state[3][3];
  state[3][3] = temp;
}

void
invShiftRows (uint8_t state[4][4]) {
  uint8_t temp;

  temp = state[1][3];
  state[1][3] = state[1][2];
  state[1][2] = state[1][1];
  state[1][1] = state[1][0];
  state[1][0] = temp;

  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;

  temp = state[3][3];
  state[3][3] = state[3][2];
  state[3][2] = state[3][1];
  state[3][1] = state[3][0];
  state[3][0] = temp;
}

void
mixColumns (uint8_t state[4][4]) {
  uint8_t temp[4];
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      temp[j] = multiply (state[0][i], constMat[j][0]) ^ 
                multiply (state[1][i], constMat[j][1]) ^ 
                multiply (state[2][i], constMat[j][2]) ^ 
                multiply (state[3][i], constMat[j][3]);
    }
    for (j = 0; j < 4; j++) {
      state[j][i] = temp[j];
    }
  }
}

void
invMixColumns (uint8_t state[4][4]) {
  uint8_t temp[4];
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      temp[j] = multiply (state[0][i], invConstMat[j][0]) ^ 
                multiply (state[1][i], invConstMat[j][1]) ^ 
                multiply (state[2][i], invConstMat[j][2]) ^ 
                multiply (state[3][i], invConstMat[j][3]);
    }
    for (j = 0; j < 4; j++) {
      state[j][i] = temp[j];
    }
  }
}

void
addRoundKey (uint8_t round, uint8_t state[4][4], uint8_t* roundKey) {
  int i, j;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[i][j] ^= roundKey[(round * Nb * 4) + (i * Nb) + j];
    }
  }
}

void
keyExpansion (uint8_t* roundKey, const uint8_t* key) {
  unsigned i, j, k;
  uint8_t temp[4];

  for (i = 0; i < Nk; i++) {
    roundKey[(i * 4) + 0] = key[(i * 4) + 0];
    roundKey[(i * 4) + 1] = key[(i * 4) + 1];
    roundKey[(i * 4) + 2] = key[(i * 4) + 2];
    roundKey[(i * 4) + 3] = key[(i * 4) + 3];
  }

  for (i = Nk; i < Nb * (Nr + 1); i++) {
    for (j = 0; j < 4; j++) {
      temp[j] = roundKey[(i - 1) * 4 + j];
    }
    if (i % Nk == 0) {
      k = temp[0];
      temp[0] = temp[1];
      temp[1] = temp[2];
      temp[2] = temp[3];
      temp[3] = k;

      temp[0] = sbox[temp[0]];
      temp[1] = sbox[temp[1]];
      temp[2] = sbox[temp[2]];
      temp[3] = sbox[temp[3]];

      temp[0] ^= rcon[i / Nk];
    }
    for (j = 0; j < 4; j++) {
      roundKey[i * 4 + j] = roundKey[(i - Nk) * 4 + j] ^ temp[j];
    }
  }
}

void
encrypt (uint8_t *input, uint8_t *output) {
  uint8_t state[4][4];
  int i, j, round;

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      state[j][i] = input[i * 4 + j];
    }
  }

  addRoundKey(0, state, roundKey);

  for (round = 1; round < Nr; round++) {
    subBytes (state);
    shiftRows (state);
    mixColumns (state);
    addRoundKey (round, state, roundKey);
  }

  subBytes (state);
  shiftRows (state);
  addRoundKey (Nr, state, roundKey);

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      output[i * 4 + j] = state[j][i];
    }
  }
}

void
decrypt (uint8_t *input, uint8_t *output) {
  uint8_t state[4][4];
  int i, j, round;

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      state[j][i] = input[i * 4 + j];
    }
  }

  addRoundKey (Nr, state, roundKey);

  for (round = Nr - 1; round >= 1; round--) {
    invShiftRows (state);
    invSubBytes (state);
    addRoundKey (round, state, roundKey);
    invMixColumns (state);
  }

  invShiftRows (state);
  invSubBytes (state);
  addRoundKey (0, state, roundKey);

  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      output[i * 4 + j] = state[j][i];
    }
  }
}

asmlinkage int
sys_aes (unsigned char*_plain, unsigned char* _cipher, int flag) {
  uint8_t plain[16], cipher[16];

  if (keyExpanded == 0) {
    keyExpansion (roundKey, key);
    keyExpanded = 1;
  }

  if (flag == AES_ENCRYPTION) {
    copy_from_user (plain, _plain, 16);
    encrypt(plain, cipher);
    copy_to_user (_cipher, cipher, 16);
  }
  else {
    copy_from_user (cipher, _cipher, 16);
    decrypt(cipher, plain);
    copy_to_user (_plain, plain, 16);
  }

  return 1;
}