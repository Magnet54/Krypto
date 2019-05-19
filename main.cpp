#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "headers/aes.h"

using namespace std;
int main() {

	char *key = "Password";
	aes Test("example\\file.txt", (uint8_t*)key);
	Test.LaunchEncryption();
	Test.GenerateFile("example\\file_encrypted.txt");
	Test.LaunchDecryption();
	Test.GenerateFile("example\\file_decrypted.txt");
	//system("pause");
	return 0;
}

