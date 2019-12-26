#include "headers/key_schedule.h"
#include <iostream>

void RotWord(uint8_t* row) {

	uint8_t row_backup[DIM];
	for (int column = 0; column < DIM; column++) {
		row_backup[column] = row[column];
		row[column] = (column + 1 >= DIM) ? row_backup[(column + 1) % DIM] : row[column + 1];
	}
}

void SubWord(uint8_t* row,uint8_t *box) {
	for (int column = 0; column < DIM; column++) {
		row[column] = box[row[column]];
	}
}

void ReplaceColumn(uint8_t input[][4],int column_nb, uint8_t* values) {
	for (int i = 0; i < DIM; i++) {
		input[i][column_nb] = values[i];
	}
}

//Find & put in array_temp column word_nb using block position
void GetWord(uint8_t current_block[][4], int word_nb, int block_nb,uint8_t* array_temp) {
	int block_on_left=0;
	while(word_nb < ((block_nb-1)*DIM)+1) {
		block_nb--;
		block_on_left++;
	}

	for (int i = 0; i < DIM; i++) {
		array_temp[i] = current_block[i-block_on_left*DIM][(word_nb-1)%DIM];
	}
}

//Main function
void KeyExpansion(uint8_t input[][4], int block_nb) {
	int word_nb=(block_nb-1)*DIM+1;
	uint8_t array_temp[4];
	uint8_t second_array_temp[4];
	while (word_nb <= block_nb*DIM) {

		GetWord(input, word_nb - 1, block_nb, array_temp);
		GetWord(input, word_nb - 8, block_nb, second_array_temp);
		//First 2 blocks
		if ((word_nb -1)% (DIM * 2) == 0) {

			RotWord(array_temp);
			SubWord(array_temp,sbox);
			array_temp[0] ^= rcon[(word_nb-1) / 8];
			


		}
		else if ((word_nb - 1) % (DIM * 2) == DIM) {
			SubWord(array_temp,sbox);

		}

		array_temp[0] ^= second_array_temp[0];
		array_temp[1] ^= second_array_temp[1];
		array_temp[2] ^= second_array_temp[2];
		array_temp[3] ^= second_array_temp[3];
		ReplaceColumn(input, (word_nb - 1) % DIM, array_temp);
		word_nb++;
	}

}
