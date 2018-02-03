FLAGS = -Wall -std=c11
CC = gcc

all: list parser

LinkedListAPI.o: src/LinkedListAPI.c
	$(CC) -Iinclude -c src/LinkedListAPI.c -o bin/LinkedListAPI.o $(FLAGS)

GEDCOMparser.o: src/GEDCOMparser.c
	$(CC) -Iinclude -c src/GEDCOMparser.c -o bin/GEDCOMparser.o $(FLAGS)

GEDCOMmutilities.o: src/GEDCOMmutilities.c
	$(CC) -Iinclude -c src/GEDCOMmutilities.c -o bin/GEDCOMparser.o $(FLAGS)

list: LinkedListAPI.o
	ar cr bin/liblist.a bin/LinkedListAPI.o

parser: GEDCOMparser.o LinkedListAPI.o GEDCOMmutilities.o
	ar cr bin/libparse.a bin/GEDCOMparser.o bin/LinkedListAPI.o bin/GEDCOMmutilities.o

clean:
	rm bin/*.o bin/*.a
