#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedListAPI.h"

#define DEBUG true

typedef struct {
  char indiRef[10];
  List famsRef;
  List famcRef;
  Individual * point;
}IndividualRef;

typedef struct{
  char famRef[10];
  char husbandRef[10];
  char wifeRef[10];
  List children;
  Family * point;
}FamilyRef;

typedef struct info{
  int level;
  char tag[255];
  char info[500];
  int line;
  int strlen;
}Info;


typedef struct{
  char indi[10];
  char FAMS[10];
  char FAMC[10];
  Individual * point;
}Reference;


typedef struct pointer{
  char addr[10];
  Individual * indiPoint;
  Family * family;
  List listPtr;
  char type[10];
}Pointer;

typedef struct {
  int level;
  Individual * indi;
}EachGen;

typedef struct{
  int level;
  List * generation;
}Generation;

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

Individual * parseIndividual(Info * record, int length, List * pointers, List * receiver, List * reference);

Family * parseFamily(Info * record, int length, List people, List * reference);

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

List collectDescendents(Individual * person, List * listOfDescendents);

bool checkSpouse(Family * fam, Individual * key);

bool checkExtistChild(List * children, Individual * search);

bool comparePerson(const void* first, const void* second);

void dummyDelete(void * toBeDeleted);

void deleteIndiReference(void * toBeDeleted);

void deleteFamReference(void * toBeDeleted);

List DescendentWrapper(Individual * person, List * lists, int var);

List AncestorWrapper(Individual * person, List * list, int var);

void deleteStruct(void * toBeDeleted);

char * printStruct(void * print);

Individual * copyIndividual(Individual * toCopy);

char * printListDescendents(void * toBePrinted);

void deleteFinalList(void * toBeDeleted);

void deleteReferenceList(void * toBeDeleted);

char * printReference(void * toBePrinted);

void deleteGenStruct(void * toBeDeleted);

ErrorCode validateField(List otherFields);

ErrorCode validateEvent(List events);

//char* toStringStar(List * list);
