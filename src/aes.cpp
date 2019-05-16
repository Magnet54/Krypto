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

	bytes_nb = bytes.size();
	cout << bytes_nb << endl;

	Block* current_block = &data;
	current_block->previous = nullptr;
	vector<char>::iterator it;
	int i = 0;
	for (it = bytes.begin(); it != bytes.end(); it++) {
		current_block->plaintext[i%DIM][i / DIM] = *it;

		i++;
		if (i >= DIM * DIM) {
			i = 0;
			current_block->next = new Block;
			current_block = current_block->next;
		}
	}

	current_block->next = nullptr;

	bytes.clear();
	key.previous = nullptr;
	Block* current_key = &key;

	for (int key_nb = 1; key_nb <= 14; key_nb++) {
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

	GenerateKey();


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
		KeyExpansion(*current_key, block_nb);
		current_key = current_key->next;

	}


}

void aes::LaunchEncryption() {

	cout << "Lancement du cryptage" << endl;

	Block* current_block = &data;

	while (current_block->next != nullptr) {
		Encrypt(*current_block);
		current_block = current_block->next;
	}

}

void aes::LaunchDecryption() {

	cout << "Lancement du decryptage" << endl;

	Block* current_block = &data;

	while (current_block->next != nullptr) {
		Decrypt(*current_block);
		current_block = current_block->next;
	}
}

void aes::Encrypt(Block& current_block) {



	Block* current_key = &key;
	AddRoundKey(current_block, *current_key);
	current_key = current_key->next;


	for (int round = 0; round < 13;round++) {
		
		SubBytes(current_block, sbox);
		ShiftRows(current_block,Forward);
		MixColumns(current_block,encrypt_matrix);
		AddRoundKey(current_block, *current_key);
		current_key = current_key->next;

	}

	SubBytes(current_block,sbox);
	ShiftRows(current_block,Forward);
	AddRoundKey(current_block, *current_key);
}

void aes::Decrypt(Block& current_block) {


	Block* current_key = &key;
	while (current_key->next != nullptr) current_key = current_key->next;
	AddRoundKey(current_block, *current_key);

	current_key = current_key->previous;

	for (int inv_round = 0; inv_round < 13; inv_round++) {
		ShiftRows(current_block, Reverse);
		SubBytes(current_block, invbox);
		AddRoundKey(current_block, *current_key);
		MixColumns(current_block, decrypt_matrix);
		current_key = current_key->previous;
	}

	ShiftRows(current_block, Reverse);
	SubBytes(current_block, invbox);
	AddRoundKey(current_block, *current_key);

}

void aes::GenerateFile(char* path) {
	ofstream encrypted_file(path, ios::out | ios::trunc | ios::binary);
	Block* current_block = &data;
	int bytes_count = bytes_nb;

	while (current_block!=nullptr)
	{
		for (int i = 0; i < DIM*DIM; i++) {
			if((bytes_count--)>0) encrypted_file << current_block->plaintext[i%DIM][i / DIM];
		}

		current_block = current_block->next;
	}


	encrypted_file.close();
}