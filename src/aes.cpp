#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <thread>
#include "../key_schedule.h"
#include "../headers/core_functions.h"

using namespace std;

aes::aes(char* path,uint8_t* raw_key) {
	ifstream raw_data(path, ios::binary);
	
	if (!raw_data) {
		cerr << "Erreur: Impossible d'ouvrir le fichier" << endl;
		exit(EXIT_FAILURE);
	}


	Block* current_block = &data;
	current_block->previous = nullptr;
	vector<char>::iterator it;

	raw_data.seekg(0, raw_data.end);
	bytes_nb = raw_data.tellg();
	raw_data.seekg(0, raw_data.beg);
	/*
	char element;

	while (raw_data.get(element)){
		current_block->plaintext[i%DIM][i / DIM] = element;
		//bytes_nb++;
		i++;
		if (i >= DIM * DIM) {
			i = 0;
			current_block->next = new Block;
			current_block = current_block->next;
		}
	}
	*/

	char buffer[1024];
	int i,n=0;
	int size = 1024;
	while (raw_data.read(buffer, size)) {
		i = 0;
		while (i < size) {
			current_block->plaintext[n%DIM][n/DIM] = buffer[i+n];
			//current_block->plaintext[1][n/DIM] = buffer[i+n];
			//current_block->plaintext[2][n/DIM] = buffer[i+2];
			//current_block->plaintext[3][n/DIM] = buffer[i+3];
			n++;

			if (n>=DIM*DIM) {
				i+=DIM*DIM;
				n = 0;
				current_block->next = new Block;
				current_block = current_block->next;
			}
		}

		if ((1024+raw_data.tellg()) > bytes_nb && raw_data.tellg()!=bytes_nb) size = bytes_nb - raw_data.tellg();

	}
	

	cout << bytes_nb << endl;
	current_block->next = nullptr;

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
	cout << "Lancement du cryptage " << endl;

	int threads_capability = thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*DIM*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;
	blocks_per_thread = bytes_nb / (threads*DIM*DIM);


	vector<thread> threads_list;
	//thread threads_list[threads];
	Block* stop_block=&data;
	Block* initial_block = &data;
	int block_nb;
	for (int i = 0; i < threads; i++) {
		block_nb = 0;
		while (block_nb < blocks_per_thread && stop_block->next!=nullptr) {
			stop_block = stop_block->next;
			block_nb++;
		}
		threads_list.push_back(thread(&aes::Encrypt,this,initial_block, stop_block));
		initial_block = stop_block->next;
	}

	for (int i = 0; i < threads;i++) {
		threads_list[i].join();

	}

}

void aes::LaunchDecryption() {

	cout << "Lancement du decryptage" << endl;

	int threads_capability =thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*DIM*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;

	vector<thread> threads_list;
	Block* stop_block=&data;
	Block* initial_block = &data;
	int block_nb;
	for (int i = 0; i < threads; i++) {

		block_nb = 0;
		while (block_nb < blocks_per_thread && stop_block->next!=nullptr) {
			stop_block =stop_block->next;
			block_nb++;
		}
		threads_list.push_back(thread(&aes::Decrypt, this, initial_block, stop_block));
		initial_block = stop_block->next;
	}

	for (int i = 0; i < threads; i++) {
		threads_list[i].join();

	}
}

void aes::Encrypt(Block* current_block,Block* stop_block) {

	Block* current_key;
	while (current_block != stop_block) {

		current_key= &key;
		AddRoundKey(*current_block, *current_key);
		current_key = current_key->next;


		for (int round = 0; round < 13; round++) {

			SubBytes(*current_block, sbox);
			ShiftRows(*current_block, Forward);
			MixColumns(*current_block, encrypt_matrix);
			AddRoundKey(*current_block, *current_key);
			current_key = current_key->next;

		}

		SubBytes(*current_block, sbox);
		ShiftRows(*current_block, Forward);
		AddRoundKey(*current_block, *current_key);

		current_block = current_block->next;
	}
}

void aes::Decrypt(Block* current_block,Block* stop_block) {

	Block* current_key;
	while (current_block != stop_block) {


		current_key = &key;
		while (current_key->next != nullptr) current_key = current_key->next;
		AddRoundKey(*current_block, *current_key);

		current_key = current_key->previous;

		for (int inv_round = 0; inv_round < 13; inv_round++) {
			ShiftRows(*current_block, Reverse);
			SubBytes(*current_block, invbox);
			AddRoundKey(*current_block, *current_key);
			MixColumns(*current_block, decrypt_matrix);
			current_key = current_key->previous;
		}

		ShiftRows(*current_block, Reverse);
		SubBytes(*current_block, invbox);
		AddRoundKey(*current_block, *current_key);

		current_block = current_block->next;
	}
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