CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -g

INCLUDES = -Iinclude

BINDIR = bin/
SRCDIR = src/

SOURCE = $(SRCDIR)main.c $(SRCDIR)LinkedList.c  $(SRCDIR)GEDCOMparser.c
BINS = $(BINDIR)LinkedList.o

all : program test

test : $(BINS) $(BINDIR)testMain.o
	$(CC) $(BINS) $(BINDIR)testMain.o -o $(BINDIR)test

program : $(BINS) $(BINDIR)main.o
	$(CC) $(BINS) $(BINDIR)main.o -o $(BINDIR)program

$(BINDIR)main.o: $(SRCDIR)main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)main.c -o $(BINDIR)main.o

$(BINDIR)testMain.o: $(SRCDIR)testMain.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)testMain.c -o $(BINDIR)testMain.o

$(BINDIR)LinkedList.o: $(SRCDIR)LinkedList.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)LinkedList.c -o $(BINDIR)LinkedList.o

$(BINDIR)GEDCOMparser.o: $(SRCDIR)GEDCOMparser.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)GEDCOMparser.c -o $(BINDIR)GEDCOMparser.o


clean:
	rm  $(BINDIR)LinkedList.o $(BINDIR)main.o $(BINDIR)testMain.o $(BINDIR)GEDCOMparser.o $(BINDIR)program $(BINDIR)test
