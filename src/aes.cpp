#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <thread>
#include <cstring>
#include "../headers/key_schedule.h"
#include "../headers/core_functions.h"


using namespace std;


Aes::Aes(const char* path,const uint8_t* password) {
	ifstream raw_data(path, ios::binary | ios::ate);
	
	if (!raw_data) {
		cerr << "Error: cannot open file" << endl;
		exit(EXIT_FAILURE);
	}


	vector<char>::iterator it;

	bytes_nb = raw_data.tellg();
	raw_data.seekg(0, raw_data.beg);

	char * buffer= new char[1024];
	int size = 1024;
	int n = 0;
	long long int block_pst=DIM*HEADER_SIZE;
	//long long int block_pst=0;
	int pst;
	this->data = new Block;
	Block* current_block = this->data;
	current_block->previous = nullptr;
	
	//Small files (<1024kb)
	if (bytes_nb < size) size = bytes_nb;



	while (raw_data.read(buffer, size)) {
		pst = 0;
		while (pst < size) {
			current_block->plaintext[block_pst+(n%DIM)][n/DIM] = buffer[pst+n];
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

		if ((1024+raw_data.tellg()) > bytes_nb && raw_data.tellg()!=bytes_nb) size = bytes_nb - raw_data.tellg();

	}
	current_block->next = nullptr;
	int current_key = 0;
	while (current_key < 2*DIM) {
		for (int i = 0; i < DIM*DIM; i++) {

			key[current_key + (i%DIM)][i / DIM] = password[(current_key*DIM + i)%strlen((char*)password)];

			raw_key[current_key + (i%DIM)][i / DIM] = password[(current_key*DIM + i)%strlen((char*)password)];
			if (i == DIM * DIM) current_key += DIM;
		}
		current_key += DIM;
	}


	raw_data.close();


	GenerateKey();


}

Aes::~Aes() {
	Block* current_block = data->next;
	Block* next_block;

	while (current_block!=nullptr)
	{
		next_block = current_block->next;
		delete current_block;
		current_block = next_block;
		
	}
}

void Aes::GenerateKey() {
	cout << "Generating key" << endl;

	for (int block_nb = 3; block_nb <= 15; block_nb++) {
		KeyExpansion(&key[(block_nb-1)*DIM], block_nb);

	}

}

void Aes::LaunchEncryption() {
	cout << "Launching encryption" << endl;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	int threads_capability = thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*BLOCK_ROWS*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;
	blocks_per_thread = bytes_nb / (threads*BLOCK_ROWS*DIM);

	vector<thread> threads_list;
	//thread threads_list[threads];
	Block* stop_block=this->data;
	Block* initial_block = this->data;
	int block_nb;

	for(int i=0;i<DIM*2;i++){
		for(int j=0;j<DIM;j++){
			
			data->plaintext[i][j] = raw_key[i][j];
		}
	}

	for (int i = 0; i < threads; i++) {
		block_nb = 0;
		while (block_nb < blocks_per_thread && stop_block->next!=nullptr) {
			stop_block = stop_block->next;
			block_nb++;
		}
		threads_list.push_back(thread(&Aes::Encrypt,this,initial_block, stop_block->next));
		initial_block = stop_block->next;
	}

	for (int i = 0; i < threads;i++) {
		threads_list[i].join();

	}
	end = chrono::system_clock::now();
	cout << "Time taken: "<< std::chrono::duration_cast<std::chrono::seconds>
		(end - start).count() << endl;
		

}

void Aes::LaunchDecryption() {

	Block* initial_block = this->data;
	Block* stop_block=this->data;


	cout << "Launching decryption" << endl;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();

	int threads_capability = thread::hardware_concurrency();
	if (threads_capability == 0) threads_capability = 1;

	int blocks_per_thread = bytes_nb / (threads_capability*BLOCK_ROWS*DIM);

	int threads = (blocks_per_thread > 1) ? threads_capability : 1;
	blocks_per_thread = bytes_nb / (threads*BLOCK_ROWS*DIM);

	vector<thread> threads_list;

	int block_nb;
	for (int i = 0; i < threads; i++) {

		block_nb = 0;
		while (block_nb < blocks_per_thread && stop_block->next!=nullptr) {
			stop_block =stop_block->next;
			block_nb++;
		}
		threads_list.push_back(thread(&Aes::Decrypt, this, initial_block, stop_block->next));

		initial_block = stop_block->next;
	}

	for (int i = 0; i < threads; i++) {
		threads_list[i].join();

	}

	end = chrono::system_clock::now();
	cout << "Time taken: " << std::chrono::duration_cast<std::chrono::seconds>
		(end - start).count() << endl;
}

void Aes::Encrypt(Block* begin_block,Block* stop_block) {

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

void Aes::Decrypt(Block* begin_block,Block* stop_block) {

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

		//Check password
		int block_row;
		if(current_block->previous==nullptr){
			for(int row=0;row<DIM*2;row++){
				
				block_row=row+HEADER_SIZE*DIM;

				for(int col=0;col<DIM;col++){
					//cout << "raw key : " << (char) raw_key[row][col] << "Block key: " << (char)current_block->plaintext[block_row][col] << endl;
					if(raw_key[row][col]!=current_block->plaintext[block_row][col]){
						cerr << "Error: Bad password" << endl;
						exit(EXIT_FAILURE);
					}
				}
			}
		}

		current_block = current_block->next;
	}
}

void Aes::GenerateFile(const char* path,bool post_treatment = false) {
	ofstream encrypted_file(path, ios::out | ios::trunc | ios::binary);

	char buffer[16];
	Block* current_block = this->data;
	int cipher = 0;
	long long int bytes_written = 0;
	int remaining;
	//Nouveau

	if(post_treatment){
		cipher=2*HEADER_SIZE;
		bytes_nb-=HEADER_SIZE*DIM*DIM;

	}else{

		bytes_nb+=DIM*DIM*HEADER_SIZE;
	
	}

	while(bytes_written < bytes_nb){

		long long int remaining=bytes_nb-bytes_written;
		for (int n = 0; n < DIM*DIM; n++) {
			buffer[n] = current_block->plaintext[cipher*DIM + (n%DIM)][n / DIM];
		}

		cipher++;
		encrypted_file.write(buffer, 16);
		bytes_written += 16;



		if(post_treatment && remaining<16){
			for(int n=0;n<remaining;n++){
				buffer[n]=current_block->plaintext[cipher*DIM + (n%DIM)][n / DIM];
			}

			break;
		}
		//Data block fully written
		if (cipher*DIM >= BLOCK_ROWS) {
			cipher = 0;
			current_block = current_block->next;
		}

	}

/*
	//Write final block
	if(bytes_nb != bytes_written){

		int remaining= bytes_nb - bytes_written;
		for(int n=0;n<remaining;n++){
			buffer[n] = current_block->plaintext[cipher*DIM + (n%DIM)][n / DIM];
		}
		encrypted_file.write(buffer, remaining);

	}
*/

	encrypted_file.close();
}
