#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include "headers/aes.h"

using namespace std;
int main(int argc, char* argv[]) {

	encrypt_way process;
	char *password;
	char *path;
	
	if (argc!=3) {
		cout << "Error: wrong arguments" << endl;
	}
	
	int count;
	for(count;count<argc;count++){
		if(argv[count].compare('-e')==0){
			process==Forward;
			cout << "Encryption mode. Please enter the new encryption password" << endl;
		}
		else if(argv[count].compare('-d')==0){
			process==Reverse;
			cout << "Decryption mode. Please enter the password for this file" << endl;
		}else{
			path=argv[count];
		}
	}
	
	cout << "Password:";	
	termios oldt;
	tcgetattr(STDIN_FILENO, &oldt);
	termios newt = oldt;
	newt.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	cin >> password;
	
	SetConsoleMode(hStdin, mode);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	
	aes Test(path, (uint8_t*)password);
	Test.LaunchEncryption();
	Test.GenerateFile("example/file_encrypted.txt");
	Test.LaunchDecryption();
	Test.GenerateFile("example/file_decrypted.txt");
	
	return 0;
}

