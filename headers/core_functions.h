#pragma once
#include "aes.h"

void SubBytes(Block& input,uint8_t *box);
void ShiftRows(Block& input);
void MixColumns(Block& input);
void AddRoundKey(Block& input,Block& key);