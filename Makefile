CC=g++
CFLAGS='-O3'

all: krypto clean

krypto: main.o key_schedule.o core_functions.o aes.o
	$(CC) $(CFLAGS) main.o key_schedule.o core_functions.o aes.o -o krypto -lpthread

main.o: src/main.cpp 
	$(CC) $(CFLAGS) -c src/main.cpp

key_schedule.o:
	$(CC) $(CFLAGS) -c src/key_schedule.cpp -o ./key_schedule.o

core_functions.o: src/core_functions.cpp
	$(CC) $(CFLAGS) -c src/core_functions.cpp -o ./core_functions.o

aes.o: src/aes.cpp
	$(CC) $(CFLAGS) -c src/aes.cpp -o aes.o

clean:
	rm -rf *.o
