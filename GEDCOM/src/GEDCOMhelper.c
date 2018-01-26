#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMmutilities.h"


Event * createEvent(char type[5], char * date, char * place, List other){
  Event * create = malloc(sizeof(Event));// creating a memory for Event

  strcpy(create->type, type);
  create->date = malloc(sizeof(char)*(strlen(date) +1));
  strcpy(create->date, date);
  create->place = malloc(sizeof(char)*(strlen(place) + 1));
  strcpy(create->place, place);
  create->otherFields = other;

  return create;
}

Field * createField(char * tag, char * value){
  Field * create = malloc(sizeof(Field));

  create->tag = malloc(sizeof(char)*(strlen(tag) + 1));
  strcpy(create->tag, tag);
  create->value = malloc(sizeof(char)*(strlen(value) + 1));
  strcpy(create->value, value);

  return create;
}

Submitter * createSubmitter(char Name[61], List other, char address[]){
  Submitter * create = malloc(sizeof(Submitter) + (sizeof(char)*strlen(address)));

  strcpy(create->submitterName, Name);
  create->otherFields = other;
  strcpy(create->address, address);

  return create;
}

Header * createHeader(char source[249], float version, CharSet encode, Submitter * submit, List other){
  Header * Head = malloc(sizeof(Header));

  strcpy(Head->source, source);
  Head->gedcVersion = version;
  Head->encoding = encode;
  Head->submitter = submit;
  Head->otherFields = other;

  return Head;
}

Individual * createIndividual(char * name, char * surname, List event, List family, List other){
  Individual * person = malloc(sizeof(Individual));

  person->givenName = malloc(sizeof(char)*(strlen(name)+1));
  strcpy(person->givenName, name);
  person->surname = malloc(sizeof(char)*(strlen(surname)+1));
  strcpy(person->surname, surname);
  person->events = event;
  person->families = family;
  person->otherFields = other;

   return person;
}

Family * createFamily(Individual * wife, Individual * husband, List children, List other){
  Family * toCreate = malloc(sizeof(Family));

  toCreate->wife = malloc(sizeof(Individual));
  toCreate->wife = wife;
  toCreate->husband = malloc(sizeof(Individual));
  toCreate->husband = husband;
  toCreate->children = children;
  toCreate->otherFields = other;

  return toCreate;
}

GEDCOMobject * createObject(Header * head, List families, List Individual, Submitter * submit){
  GEDCOMobject * obj = malloc(sizeof(GEDCOMobject));

  obj->header = malloc(sizeof(Header));
  obj->header = head;
  obj->families = families;
  obj->individuals = Individual;
  obj->submitter = malloc(sizeof(Submitter));
  obj->submitter = submit;

  return obj;
}

void freeEvent(Event * toBeFreed);

void freeField(Field * toBeFreed);

void freeSubmitter(Submitter * toBeFreed);

void freeHeader(Header * toBeFreed);

void freeIndividual(Individual * toBeFreed);

void freeFmaily(Family * toBeFreed);

void freeOject(GEDCOMobject * toBeFreed);

void toLower(char * toBeLower);

GEDCOMerror setType(ErrorCode code, int line){
  GEDCOMerror temp;
  temp.type = code;
  temp.line = line;
  return temp;
}

void toLower(char * toBeLower){
  while(*toBeLower)
   {
      if ( *toBeLower >= 'A' && *toBeLower <= 'Z' )
      {
         *toBeLower = *toBeLower + 32;
      }
      toBeLower++;
   }
}

int getMonth(char * getter){
  char month[12][5] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

  for (int i = 0; i < 12; i++){
    if (strcmp(month[i], getter)){
      return i + 1;
    }
  }
    return 0;
}


enum eCode validateFile(char* fileName){
  printf("in the function\n");
  //char ext[80];
  if (strlen(fileName) == 0){ // checking if the file argument is empty, file is invalid
    printf("empty file name\n");
    return INV_FILE;
  }
  FILE * file = fopen(fileName, "r");
  if (file == NULL){
    printf("file is NULL, empty\n");
    return INV_FILE;
    //return NULL;
  }
  fclose(file);
  //ext = strrchr(fileName, '.');
  return OK;
}

enum eCode validateGEDCOM(char * read){
  //int length = fileLength(read);
  //int term;


  return OK;
}

int checkTerminate(char * string){
  for (int i = 0; i < strlen(string); i++){
    if (string[i] == '\r' || string[i] == '\n'){//when the terminator is either line feed or carriage return
      return 1;
    }
    else if (string[i] == '\r' && string[i+1] == '\n'){
      return 1; // when line feed + carriage return
    }
    else if (string[i] == '\n' && string[i+1] == '\r'){
      return 1; // when carraige return + line feed
    }
  }
  return 0; // when nothing fits in the condition
}

char** fileReader(char * fileName){
  char** array = (char**)malloc(sizeof(char*)*1);
  char reader[1000];
  int counter = 1;
  int i = 0;
  FILE * file = fopen(fileName, "r");
  if(DEBUG) printf("\nopening the file\n");

  while(fgets(reader, 800, file) != NULL){
    //if(DEBUG)printf("\nread a line \n");

    array = (char**)realloc(array, sizeof(char*)*counter);
    while(reader[strlen(reader) - 1] == '\n' || reader[strlen(reader) - 1] =='\r'){
      reader[strlen(reader) - 1] = '\0';
    }

    array[i] = (char*)malloc(sizeof(char) * 253);
    strcpy(array[i], reader);
    //printf("reading in file: %s\n", array[i]);
    printf("\n\n");
    i++;
    counter++;
  }

  fclose(file);
  return array;
}

int fileLength(char ** array){
  int i = 0;
  if (DEBUG)printf("\ngetting the length of file\n");
  while(array[i] != NULL){
    i++;
  }
  //printf("number of element in array %d\n", i);
  return i;
}

Info tockenInfo(char * toParse){ //parses the line of GEDCOM and saves into temporary info struct
  Info info;
  int i = 0;
  int j = 0;
  //char temp[500];
  //char level[5];
  //char tag[500];
  char information[500] = "";
  char parsed[50][255];
  printf("parsing this line : %s\n", toParse);
  char * token = strtok(toParse, " ");

  while(token != NULL){
    //printf("%s\n", token);
    strcpy(parsed[i], token);
    printf("tockenized <%s>\n", parsed[i]);
    token = strtok(NULL, " ");
    i++;
  }
  if (i > 1){
    while(strcmp(parsed[j], "\0") == 0){
      strcat(information, parsed[j]);
      strcat(information, " ");
    }
  }


  //updated
  /*while(toParse[i] !=  ' '){// iterating the first white space
    int j = 0;
    level[j] = toParse[i];
    i++;// iterates until it finds the first white space
    j++;
  }
  printf("i = %d || level: %s || %s\n", i, level, toParse);
  i++;//increments that it won;t count the white space that the index is currently pointing at
  while(toParse[i] != ' '){// iterating till the second white space
    int j = 0;
    tag[i] = temp[i]; // saving the characters in the tag
    i++;
    j++;
  }
  printf(" i = %d || tag : %s ||  %s\n", i, tag, toParse);
  i++;
  while(temp[i] != '\0'){
    int j = 0;
    information[i] = temp[i]; //saving the rest of the info into information
    i++;
    j++;
  }
  printf("i = %d || info %s || %s\n",i, information, temp);*/



  strcpy(info.level, parsed[0]);
  strcpy(info.tag, parsed[1]);
  strcpy(info.info, information);
  printf("[[%s %s %s]]\n", info.level, info.tag, info.info);

  return info;
}
