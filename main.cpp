#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include "headers/aes.h"

using namespace std;
int main(int argc, char* argv[]) {

	encrypt_way process;
	char *password;
	char *path;
	if (argc!=5) {
		cout << "Error: wrong arguments" << endl;
	}
	
	int count;
	for(count;count<argc;count++){
		if(argv[count].compare('-e')==0){
			process==Forward;
		}
		else if(argv[count].compare('-d')==0){
			process==Reverse;
		}
		else if(argv[count].compare('-p')==0){
			password=argv[count+1];
		}else{
			path=argv[count];
		}
	
	aes Test(path, (uint8_t*)password);
	Test.LaunchEncryption();
	Test.GenerateFile("example/file_encrypted.txt");
	Test.LaunchDecryption();
	Test.GenerateFile("example/file_decrypted.txt");
	
	return 0;
}

