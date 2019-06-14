#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <thread>
#include <cstring>
#include "../headers/key_schedule.h"
#include "../headers/core_functions.h"

using namespace std;

aes::aes(char* path,const uint8_t* raw_key) {
	ifstream raw_data(path, ios::binary);
	
	if (!raw_data) {
		cerr << "Error: cannot open file" << endl;
		exit(EXIT_FAILURE);
	}

	//Get file size
	raw_data.seekg(0, raw_data.end);
	bytes_nb = raw_data.tellg();
	raw_data.seekg(0, raw_data.beg);

	int size = 1024;	//Buffer size
	char buffer[size];	//Reading buffer
	int n = 0;
	int pst,block_pst=0;
	
	this->data = new Block;
	Block* current_block = data;
	current_block->previous = nullptr;
	
	if (bytes_nb < size) size = bytes_nb;	//For files < 1024 bytes
	
	while (raw_data.read(buffer, size)) {
		pst = 0;
		while (pst < size) {
			current_block->plaintext[block_pst+(n%DIM)][n/DIM] = buffer[pst+n];	//Write file data in memory
			n++;

			if (n>=DIM*DIM) {
				block_pst+=DIM;
				pst += DIM * DIM;
				n = 0;
				if (block_pst >= BLOCK_ROWS) {
					block_pst = 0;
					current_block->next = new Block;
					current_block = current_block->next;
				}
			}
		}

		//Avoid buffer overflow
		if ((size+raw_data.tellg()) > bytes_nb && raw_data.tellg()!=bytes_nb) size = bytes_nb - raw_data.tellg();

	}
	
	//Generate first 2 blocks of the key using user password (raw key)
	current_block->next = nullptr;
	int current_key = 0;
	while (current_key < 2*DIM) {
		for (int i = 0; i < DIM*DIM; i++) {

			key[current_key + (i%DIM)][i / DIM] = raw_key[(current_key*DIM + i)%strlen((char*)raw_key)];
			if (i == DIM * DIM) current_key += DIM;
		}
		current_key += DIM;
	}


	raw_data.close();

	cout << "File successfully loaded" << endl;

	GenerateKey();


}

aes::~aes() {
	Block* current_block = data->next;
	Block* next_block;

	while (current_block!=nullptr)
	{
		next_block = current_block->next;
		delete current_block;
		current_block = next_block;
		
	}
}

void aes::GenerateKey() {
	cout << "Generating key" << endl;

	for (int block_nb = 3; block_nb <= 15; block_nb++) {
		KeyExpansion(&key[(block_nb-1)*DIM], block_nb);

	}

}

/*
	LaunchEncryption() initiates document encryption 
	by dividing data into multiple block lists for multithreading
*/
void aes::LaunchEncryption() {
	cout << "Launching encryption" << endl;
	
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	
	int threads_capability = thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*BLOCK_ROWS*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;
	blocks_per_thread = bytes_nb / (threads*BLOCK_ROWS*DIM);


	vector<thread> threads_list;
	Block* stop_block=this->data;
	Block* initial_block = this->data;
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
	
	end = chrono::system_clock::now();
	cout << "Encryption time: "<< std::chrono::duration_cast<std::chrono::seconds>
		(end - start).count() << endl;

}

void aes::LaunchDecryption() {

	cout << "Lancement du decryptage" << endl;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();

	int threads_capability = thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*BLOCK_ROWS*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;
	blocks_per_thread = bytes_nb / (threads*BLOCK_ROWS*DIM);

	vector<thread> threads_list;
	Block* stop_block=this->data;
	Block* initial_block = this->data;
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

	end = chrono::system_clock::now();
	cout << "Temps cryptage: " << std::chrono::duration_cast<std::chrono::seconds>
		(end - start).count() << endl;
}

void aes::Encrypt(Block* begin_block,Block* stop_block) {

	Block* current_block = begin_block;
	int current_key;
	while (current_block==begin_block || ( current_block != stop_block && current_block!=nullptr )) {
		for (int cipher = 0; cipher < BLOCK_ROWS; cipher += DIM) {

			current_key = 0;
			AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);
			current_key +=DIM;


			for (int round = 0; round < 13; round++) {

				SubBytes(&current_block->plaintext[cipher], sbox);
				ShiftRows(&current_block->plaintext[cipher], Forward);
				MixColumns(&current_block->plaintext[cipher], encrypt_matrix);
				AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);
				current_key += DIM;

			}

			SubBytes(&current_block->plaintext[cipher], sbox);
			ShiftRows(&current_block->plaintext[cipher], Forward);
			AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);


		}
		current_block = current_block->next;

	}
}

void aes::Decrypt(Block* begin_block,Block* stop_block) {

	int current_key;
	Block* current_block = begin_block;
	while (current_block == begin_block || (current_block != stop_block && current_block != nullptr)) {
		for (int cipher = 0; cipher < BLOCK_ROWS; cipher += DIM) {

			current_key = 14*DIM;
			AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);

			current_key-=DIM;

			for (int inv_round = 0; inv_round < 13; inv_round++) {
				ShiftRows(&current_block->plaintext[cipher], Reverse);
				SubBytes(&current_block->plaintext[cipher], invbox);
				AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);
				MixColumns(&current_block->plaintext[cipher], decrypt_matrix);
				current_key -=DIM;
			}

			ShiftRows(&current_block->plaintext[cipher], Reverse);
			SubBytes(&current_block->plaintext[cipher], invbox);
			AddRoundKey(&current_block->plaintext[cipher], &key[current_key]);

		}
		current_block = current_block->next;
	}
}

void aes::GenerateFile(char* path) {
	ofstream encrypted_file(path, ios::out | ios::trunc | ios::binary);

	char buffer[16];
	Block* current_block = this->data;
	int cipher = 0;
	int bytes_written = 0;

	while (current_block!=nullptr) {
		for (int n = 0; n < DIM*DIM; n++) {
			buffer[n] = current_block->plaintext[cipher*DIM + (n%DIM)][n / DIM];
		}
		cipher++;

		if (bytes_nb-bytes_written<16) {
			encrypted_file.write(buffer, bytes_nb-bytes_written);
			break;
		}
		encrypted_file.write(buffer, 16);
		bytes_written += 16;
		if (cipher*DIM >= BLOCK_ROWS) {
			cipher = 0;
			current_block = current_block->next;
		}
	}


	encrypted_file.close();
}
