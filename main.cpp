#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "headers/aes.h"

using namespace std;
int main() {

	char *key = "password";
	aes Test("example/image.jpg", (uint8_t*)key);
	Test.LaunchEncryption();
	Test.GenerateFile("example/file_encrypted.jpg");
	Test.LaunchDecryption();
	Test.GenerateFile("example/file_decrypted.jpg");
	//system("pause");
	return 0;
}

