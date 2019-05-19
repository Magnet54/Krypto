#include "../headers/core_functions.h"
#include <iostream>

void SubBytes(uint8_t input[][4],uint8_t *box) {
	for (int position = 0; position < DIM*DIM; position++) {
		input[position%DIM][position / DIM] = box[input[position%DIM][position / DIM]];
	}
}

void ShiftRows(uint8_t input[][4],encrypt_way way) {

	uint8_t row_backup[DIM];
	int remainder;

	for (int row = 0; row < DIM; row++) {
		if (way == Forward) {

			for (int column = 0; column < DIM; column++) {
				row_backup[column] = input[row][column];
				input[row][column] = (column + row >= DIM) ? row_backup[(column + row) % DIM] : input[row][column + row];
			}
		}
		else {
			for (int column = DIM-1; column >= 0; column--) {
				row_backup[column] = input[row][column];
				remainder = ((column - row) % DIM + DIM) % DIM;
				input[row][column] = (column - row < 0) ? row_backup[remainder] : input[row][column - row];
			}
		}



	}
}

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

void MixColumns(uint8_t input[][4],uint8_t matrix[][4]) {
	uint8_t temp[4][4];


	for (int i = 0; i < DIM*DIM; i++) {
		temp[i%DIM][i / DIM] = gf_multiplication(matrix[i%DIM][0], input[0][i / DIM])
			^ gf_multiplication(matrix[i%DIM][1], input[1][i / DIM])
			^ gf_multiplication(matrix[i%DIM][2], input[2][i / DIM])
			^ gf_multiplication(matrix[i%DIM][3], input[3][i / DIM]);

	}

	std::copy(&temp[0][0], &temp[3][4], &input[0][0]);

}

void AddRoundKey(uint8_t input[][4],uint8_t key[][4]) {
	for (int i = 0; i < DIM*DIM; i++) {
		input[i%DIM][i / DIM] ^= key[i%DIM][i / DIM];
	}
}