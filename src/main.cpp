#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "../headers/aes.h"

using namespace std;
int main() {
	/*
	ifstream input("C:\\arret.pdf",ios::binary);
	ofstream encrypted_file("C:\\Temp\\encrypt.pdf", ios::out | ios::trunc | ios::binary);

	vector<char> bytes(
		(istreambuf_iterator<char>(input)),
		(istreambuf_iterator<char>()));



	Block B1;
	cout << "10 premiers octets" << endl;
	for (int i = 0; i < DIM*DIM; i++) {
		B1.plaintext[i / DIM][i%DIM] = bytes[i];
		cout << B1.plaintext[i / DIM][i%DIM];
	}
	cout << endl;
	cout << "Apres passage SBox" << endl;

	SubBytes(B1,sbox);
	ShiftRows(B1);
	MixColumns(B1);
	//SubBytes(B1, invbox);

	for (int i = 0; i < DIM*DIM; i++) {
		encrypted_file << B1.plaintext[i / DIM][i%DIM];
		cout << B1.plaintext[i / DIM][i%DIM] << endl;
	}

	input.close();
	encrypted_file.close();
	*/
	uint8_t key[32] = { 
		0x00,0x01,0x02,0x03,
		0x04,0x05,0x06,0x07,
		0x08,0x09,0x0a,0x0b,
		0x0c,0x0d,0x0e,0x0f,
		
		0x10,0x11,0x12,0x13,
		0x14,0x15,0x16,0x17,
		0x18,0x19,0x1a,0x1b,
		0x1c,0x1d,0x1e,0x1f };

	aes Test("C:\\input", key);
	Test.GenerateKey();
	Test.Encrypt();
	Test.Decrypt();
	Test.GenerateFile("C:\\Temp\\encrypt.pdf");
	//system("pause");
	return 0;
}

