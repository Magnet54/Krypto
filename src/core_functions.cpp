#include "./headers/core_functions.h"
#include <iostream>

void SubBytes(uint8_t input[][4],uint8_t *box) {
	for (int row= 0; row < DIM; row++) {
		for(int column=0;column<DIM;column++){

			input[row][column] = box[input[row][column]];
		}
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
	bool high_bit;
	for (int position = 0; position < 8; position++) {

		if ((x & 1) != 0) {
			result ^= mds_matrix_value;
		}

		high_bit = (mds_matrix_value & 0x80) != 0;
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


	for (int i = 0; i < DIM; i++) {
		for(int j=0;j<DIM;j++){

			temp[i][j] = gf_multiplication(matrix[i][0], input[0][j])
				^ gf_multiplication(matrix[i][1], input[1][j])
				^ gf_multiplication(matrix[i][2], input[2][j])
				^ gf_multiplication(matrix[i][3], input[3][j]);			
		}


	}

	std::copy(&temp[0][0], &temp[3][4], &input[0][0]);

}

void AddRoundKey(uint8_t input[][4],uint8_t key[][4]) {
	for (int i = 0; i < DIM; i++) {
		for(int j=0;j<DIM;j++){
			input[i][j] ^= key[i][j];
		}


	}
}