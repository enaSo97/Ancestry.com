CC = gcc
CFLAGS = -std=c11 -Wall -pedantic -Iinclude -g

all: test

test:
	$(CC) $(CFLAGS) src/GEDCOMutilities.c src/GEDCOMparser.c src/LinkedListAPI.c src/testMain.c -o bin/test