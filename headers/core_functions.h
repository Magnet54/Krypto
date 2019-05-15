#pragma once
#include "aes.h"

void SubBytes(Block& input,uint8_t *box);
void ShiftRows(Block& input,encrypt_way way);
void MixColumns(Block& input,uint8_t matrix[][4]);
void AddRoundKey(Block& input,Block& key);