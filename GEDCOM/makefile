CC = gcc
CFLAGS = -std=c11 -Wall -pedantic -Iinclude -g

all: test

test:
	$(CC) $(CFLAGS) src/GEDCOMhelper.c src/GEDCOMparser.c src/LinkedList.c src/testMain.c -o bin/test
