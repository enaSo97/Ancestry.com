#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedListAPI.h"

#define DEBUG true

Event * createEvent(char type[5], char * date, char * place, List other);

Field * createField(char * tag, char * value);

Submitter * createSubmiter(char Name[61], List other, char address[]);

Header * createHeader(char source[249], float version, CharSet encode, Submitter * submit, List other);

Individual * createIndividual(char * name, char * surname, List event, List family, List other);

Familty * createFamily(Individual * wife, Individual * husband, List children, List other);

GEDCOMobject * createObject(Header * head, List families, List Individual, Submitter * submit);

GEDCOMerror setType(ErrorCode code, int line);

void freeEvent(Event * toBeFreed);

void freeField(Field * toBeFreed);

void freeSubmitter(Submitter * toBeFreed);

void freeHeader(Head * toBeFreed);

void freeIndividual(Individual * toBeFreed);

void freeFmaily(Fmaily * toBeFreed);

void freeOject(GEDCOMobject * toBeFreed);

void toLower(char * toBeLower);

enum eCode validateFile(char* fileName);

enum eCode validateGEDCOM(char ** fileName);

char** fileReader(char * fileName);

int fileLength(char ** array);

int checkTerminate(char * string);
