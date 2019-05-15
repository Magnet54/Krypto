#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "../key_schedule.h"
#include "../headers/core_functions.h"

using namespace std;

aes::aes(char* path,uint8_t* raw_key) {
	ifstream raw_data(path, ios::binary);
	vector<char> bytes(
		(istreambuf_iterator<char>(raw_data)),
		(istreambuf_iterator<char>()));

	data.previous = nullptr;
	for (int i = 0; i < DIM*DIM; i++) {
		data.plaintext[i % DIM][i/DIM] = bytes[i];
	}

	key.previous = nullptr;
	Block* current_key = &key;

	for (int key_block_nb = 1; key_block_nb <= 14; key_block_nb++) {
		current_key->next = new Block;
		current_key->next->previous = current_key;
		current_key = current_key->next;
	}

	current_key->next = nullptr;
	int block_nb = 1;
	current_key = &key;
	for (int i = 0; i < DIM*DIM*2; i++) {
		if (i >= DIM * DIM*block_nb) {
			current_key = current_key->next;
			block_nb++;
		}
		current_key->plaintext[i%DIM][(i%(DIM*DIM)) / DIM] = raw_key[i];

	}

	raw_data.close();

	cout << "Le fichier a été chargé" << endl;


}

aes::~aes() {
	//for (Block* block_ptr = &key; block_ptr != nullptr; block_ptr = block_ptr->next) {
	//	delete block_ptr;
	//}
}

void aes::GenerateKey() {
	cout << "Génération de la clé" << endl;

	Block* current_key = (key.next->next);

	for (int block_nb = 3; block_nb <= 15; block_nb++) {
		cout << "Bloc n" << block_nb << endl;
		KeyExpansion(*current_key, block_nb);
		current_key = current_key->next;

	}
	cout << "Clé générée" << endl;

	current_key = &key;
	Block test;
	uint8_t array[DIM];
	//GetWord(current_key, 8, 3, array);
	//RotWord(array);
	//SubWord(array, sbox);
	//array[0] ^= rcon[(9 - 1) / 8];
	//ReplaceColumn(test, 0, array);
	ofstream fichier("C:\\Temp\\key", ios::out | ios::trunc | ios::binary);
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 4; j++) {
			fichier << current_key->plaintext[0][j] 
				<< current_key->plaintext[1][j] 
				<< current_key->plaintext[2][j] 
				<< current_key->plaintext[3][j];
		}
		current_key = current_key->next;
	}

	fichier.close();


}

void aes::Encrypt() {

	cout << "Lancement de la procédure de cryptage" << endl;

	Block* current_key = &key;
	AddRoundKey(data, *current_key);
	current_key = current_key->next;


	for (int round = 0; round < 13;round++) {
		
		cout << "Ronde: " << round << endl;
		SubBytes(data, sbox);
		ShiftRows(data,Forward);
		MixColumns(data,encrypt_matrix);
		AddRoundKey(data, *current_key);
		current_key = current_key->next;

	}

	SubBytes(data,sbox);
	ShiftRows(data,Forward);
	AddRoundKey(data, *current_key);
}

void aes::Decrypt() {
	cout << "Lancement de la procédure de decryptage" << endl;

	Block* current_key = &key;
	while (current_key->next != nullptr) current_key = current_key->next;
	AddRoundKey(data, *current_key);

	current_key = current_key->previous;

	for (int inv_round = 0; inv_round < 13; inv_round++) {
		ShiftRows(data, Reverse);
		SubBytes(data, invbox);
		AddRoundKey(data, *current_key);
		MixColumns(data, decrypt_matrix);
		current_key = current_key->previous;
	}

	ShiftRows(data, Reverse);
	SubBytes(data, invbox);
	AddRoundKey(data, *current_key);

}

void aes::GenerateFile(char* path) {
	ofstream encrypted_file(path, ios::out | ios::trunc | ios::binary);
	for (int i = 0; i < DIM*DIM; i++) {
		encrypted_file << data.plaintext[i%DIM][i / DIM];
	}

	encrypted_file.close();
}