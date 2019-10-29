CC=g++
CFLAGS='-g'

all: launcher clean

launcher: main.o key_schedule.o core_functions.o aes.o
	$(CC) $(CFLAGS) main.o key_schedule.o core_functions.o aes.o -o launcher -lpthread

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp

key_schedule.o:
	$(CC) $(CFLAGS) -c src/key_schedule.cpp -o ./key_schedule.o

core_functions.o: src/core_functions.cpp
	$(CC) $(CFLAGS) -c src/core_functions.cpp -o ./core_functions.o

aes.o: src/aes.cpp
	$(CC) $(CFLAGS) -c src/aes.cpp -o aes.o

clean:
	rm -rf *.o
