#include <iostream>
#include <fstream>
#include <iterator>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include "headers/aes.h"

using namespace std;

void SetDisplayCin(bool enable = true)
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}


int main(int argc, char *argv[]) {

	if(argc < 3){
		cout << "Error: accepting only 2 parameters" << endl;
		return 1;
	}

	string input_password;

	if(isatty(fileno(stdin))){
		cout << "Enter password:" << endl << "> ";
		SetDisplayCin(false);

	}


	cin >> input_password;

	SetDisplayCin(true);

	const char *key = input_password.c_str();
	Aes Main(argv[2], (uint8_t*)key);
	string filename(argv[2]);
	bool post_treatment;

	if(strcmp(argv[1], "-e")==0){

		Main.LaunchEncryption();
		filename += ".encrypted";
		post_treatment=false;

	}else if(strcmp(argv[1], "-d")==0){

		string extens(".encrypted");
		size_t pst = filename.find(extens);
		filename.erase(pst,filename.size());
		post_treatment=true;

		Main.LaunchDecryption();

	}else{
		cout << "Error: unrecognized parameter" << endl;
		return 1;
	}

	if(remove(argv[2])!=0) cout << "Error when deleting original file" << endl;
	Main.GenerateFile(filename.c_str(),post_treatment);

	return 0;
}

