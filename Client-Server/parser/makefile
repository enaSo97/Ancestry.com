CC = gcc
CFLAGS = -std=c11 -Wall -g

LIB = shareLib.so

SRC = ./src/
BIN = ../
INC = ./include/

all: $(BIN)LinkedListAPI.o $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)A3helper.o
	$(CC) -shared -o $(BIN)libparser.so $(BIN)LinkedListAPI.o $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)A3helper.o

parser: $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)A3helper.o
	ar rcs $(BIN)libparser.a $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o $(BIN)A3helper.o
	
list: $(BIN)LinkedListAPI.o
	ar rcs $(BIN)libparser.a $(BIN)LinkedListAPI.o
	
clean:
	rm -f $(BIN)*
	
$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c
	$(CC) $(CFLAGS) -c $(SRC)LinkedListAPI.c -fpic -Iinclude -o $(BIN)LinkedListAPI.o

$(BIN)GEDCOMparser.o: $(SRC)GEDCOMparser.c
	$(CC) $(CFLAGS) -c $(SRC)GEDCOMparser.c -fpic -Iinclude -o $(BIN)GEDCOMparser.o
	
$(BIN)GEDCOMutilities.o: $(SRC)GEDCOMutilities.c
	$(CC) $(CFLAGS) -c $(SRC)GEDCOMutilities.c -fpic -Iinclude -o $(BIN)GEDCOMutilities.o

$(BIN)A3helper.o: $(SRC)A3helper.c
	$(CC) $(CFLAGS) -c $(SRC)A3helper.c -fpic -Iinclude -o $(BIN)A3helper.o
