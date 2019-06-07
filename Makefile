all: launcher clean

launcher: main.o key_schedule.o core_functions.o aes.o
	g++ -O3 main.o key_schedule.o core_functions.o aes.o -o launcher -lpthread

main.o: main.cpp 
	g++ -O3 -Wall -c main.cpp

key_schedule.o:
	g++ -O3 -c src/key_schedule.cpp -o ./key_schedule.o

core_functions.o: src/core_functions.cpp
	g++ -O3 -c src/core_functions.cpp -o ./core_functions.o

aes.o: src/aes.cpp
	g++ -O3 -Wall -c src/aes.cpp -o aes.o

clean:
	rm -rf *.o
