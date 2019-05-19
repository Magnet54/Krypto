#pragma once
#include "aes.h"

void SubBytes(uint8_t input[][4],uint8_t *box);
void ShiftRows(uint8_t input[][4],encrypt_way way);
void MixColumns(uint8_t input[][4],uint8_t matrix[][4]);
void AddRoundKey(uint8_t input[][4],uint8_t key[][4]);