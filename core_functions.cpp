#include "headers/core_functions.h"
#include <iostream>

void SubBytes(Block &input,uint8_t *box) {
	for (int position = 0; position < DIM*DIM; position++) {
		input.plaintext[position%DIM][position / DIM] = box[input.plaintext[position%DIM][position / DIM]];
	}
}

void ShiftRows(Block& input,encrypt_way way) {

	uint8_t row_backup[DIM];
	int remainder;

	for (int row = 0; row < DIM; row++) {
		if (way == Forward) {

			for (int column = 0; column < DIM; column++) {
				row_backup[column] = input.plaintext[row][column];
				input.plaintext[row][column] = (column + row >= DIM) ? row_backup[(column + row) % DIM] : input.plaintext[row][column + row];
			}
		}
		else {
			for (int column = DIM-1; column >= 0; column--) {
				row_backup[column] = input.plaintext[row][column];
				remainder = ((column - row) % DIM + DIM) % DIM;
				input.plaintext[row][column] = (column - row < 0) ? row_backup[remainder] : input.plaintext[row][column - row];
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

void MixColumns(Block& input,uint8_t matrix[][4]) {
	Block temp = input;


	for (int i = 0; i < DIM*DIM; i++) {
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