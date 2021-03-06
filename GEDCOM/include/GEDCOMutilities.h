#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedListAPI.h"

#define DEBUG true

typedef struct info{
  int level;
  char tag[255];
  char info[255];
  int line;
  int strlen;
}Info;

typedef struct pointer{
  char addr[10];
  Individual * indiPoint;
  Family * family;
  List listPtr;
  char type[10];
}Pointer;

/*typedef struct indiPoint{
  char addr[10];
  Individual * indi;
  char type[10];
}

typedef struct famPoint{
  char addr[10];
  Family * fam;
  char type[10];
}*/

Event * createEvent(char type[5], char * date, char * place, List other);

Field * createField(char tag[20], char value[255]);

Submitter * createSubmiter(char Name[61], List other, char address[]);

Header * createHeader(char source[249], float version, CharSet encode, Submitter * submit, List other);

Individual * createIndividual(char * name, char * surname, List event, List family, List other);

Family * createFamily(Individual * wife, Individual * husband, List children, List other);

GEDCOMobject * createObject(Header * head, List families, List Individual, Submitter * submit);

GEDCOMerror setType(ErrorCode code, int line);

void freeEvent(Event * toBeFreed);

void freeField(Field * toBeFreed);

void freeSubmitter(Submitter * toBeFreed);

void freeHeader(Header * toBeFreed);

void freeIndividual(Individual * toBeFreed);

void freeFmaily(Family * toBeFreed);

void freeOject(GEDCOMobject * toBeFreed);

void toLower(char * toBeLower);

enum eCode validateFile(char* fileName);

enum eCode validateRecord(Info * record, int length);

enum eCode validateHeader(Info * record, int length);

char** fileReader(char * fileName);

int fileLength(char ** array);

int checkTerminate(char * string);

void testTockenize(char * tocken);

Info tockenInfo(char * toParse);

Header * headParser(Info * record, int length, List * pointers, List * receiver);

Submitter * subParser(Info * record, int length, List * pointers, List * receiver);

Individual * parseIndividual(Info * record, int length, List * pointers, List * receiver);

Family * parseFamily(Info * record, int length, List people);

char * printFunc(void * data);

void deleteFunc(void * erase);

int compare(const void *first,const void *second);

char * printPointers(void * data);

void deletePointers(void * erase);

bool comparePointers(const void *first,const void *second);

int dummyCompare(const void *first,const void *second);

int validateIndividualEvent(char * check);

int validateFamilyEvent(char * check);

void linkerFunction(List * pointer, void * data);

char * returnString(char * data);

//void enumToString(ErrorCode error);

bool validateTags(char * check);


