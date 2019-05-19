#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "../headers/aes.h"

using namespace std;
int main() {

	char *key = "Password";
	aes Test("C:\\Temp\\file.exe", (uint8_t*)key);
	Test.LaunchEncryption();
	Test.GenerateFile("C:\\Temp\\file_encrypted.exe");
	//Test.LaunchDecryption();
	//Test.GenerateFile("C:\\Temp\\file_decrypted.exe");
	system("pause");
	return 0;
}

