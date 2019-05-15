#include "headers/core_functions.h"
#include <iostream>

void SubBytes(Block &input,uint8_t *box) {
	for (int position = 0; position < DIM*DIM; position++) {
		input.plaintext[position%DIM][position / DIM] = box[input.plaintext[position%DIM][position / DIM]];
	}
}

void ShiftRows(Block& input) {

	uint8_t row_backup[DIM];
	for (int row = 0; row < DIM; row++) {

		for (int column = 0; column < DIM; column++) {
			row_backup[column] = input.plaintext[row][column];
			input.plaintext[row][column] = (column + row >= DIM) ? row_backup[(column + row) % DIM] : input.plaintext[row][column + row];
		}


	}
}

/*
Reference: Kit Choy Xintong
Understanding AES Mix-Columns Transformation Calculation
*/
/*
uint8_t gf_multiplication(uint8_t mds_matrix_value, uint8_t x) {

	uint8_t result = 0;
	if (mds_matrix_value == 0x03) {
		result = gf_multiplication(0x02, x) ^ x;
	}
	else if (mds_matrix_value == 0x02) {
		result = x << 1;
	}
	else {
		result = x;
	}

	if ((x & 0x80)!=0) return (result ^ 0x11b);

	return result;

}
*/

uint8_t gf_multiplication(uint8_t mds_matrix_value, uint8_t x) {

	uint8_t result = 0;

	for (int position = 0; position < 8; position++) {

		if ((x & 1) != 0) {
			result ^= mds_matrix_value;

		}

		bool high_bit = (mds_matrix_value & 0x80) != 0;
		mds_matrix_value <<= 1;

		if (high_bit) {

			mds_matrix_value ^= 0x11B;
		}

		x >>= 1;

	}

	return result;

}

void MixColumns(Block& input) {
	Block temp = input;

	uint8_t matrix[4][4] = {
	{0x02,0x03,0x01,0x01},
	{0x01,0x02,0x03,0x01},
	{0x01,0x01,0x02,0x03},
	{0x03,0x01,0x01,0x02}
	};

	for (int i = 0; i < DIM*DIM; i++) {
		std::cout << "-----------" << std::endl;
		input.plaintext[i%DIM][i / DIM] = gf_multiplication(matrix[i%DIM][0], temp.plaintext[0][i / DIM])
			^ gf_multiplication(matrix[i%DIM][1], temp.plaintext[1][i / DIM])
			^ gf_multiplication(matrix[i%DIM][2], temp.plaintext[2][i / DIM])
			^ gf_multiplication(matrix[i%DIM][3], temp.plaintext[3][i / DIM]);

	}


}

void AddRoundKey(Block& input,Block& key) {
	for (int i = 0; i < DIM*DIM; i++) {
		input.plaintext[i%DIM][i / DIM] ^= key.plaintext[i%DIM][i / DIM];
	}
}