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
  char** array = (char**)malloc(sizeof(char*)*10000000);
  char reader[1000];
  int counter = 1;
  int i = 0;
  FILE * file = fopen(fileName, "r");
  if(DEBUG) printf("\nopening the file\n");

  while(fgets(reader, 800, file) != NULL){
    //if(DEBUG)printf("\nread a line \n");

    //array = (char**)realloc(array, sizeof(char*)*counter);
    while(reader[strlen(reader) - 1] == '\n' || reader[strlen(reader) - 1] =='\r'){
      reader[strlen(reader) - 1] = '\0';
    }

    array[i] = (char*)malloc(sizeof(char) * 253);
    strcpy(array[i], reader);
    //printf("reading in file: %s\n", array[i]);
    i++;
    counter++;
  }

  fclose(file);
  return array;
}

int fileLength(char ** array){
  int i = 0;
  char ** temp = array;
  if (DEBUG)printf("\ngetting the length of file\n");

  while(temp[i] != NULL){
    i++;
  }
  //printf("number of element in array %d\n", i);
  return i;
}

Info tockenInfo(char * toParse){ //parses the line of GEDCOM and saves into temporary info struct
  Info info;
  int i = 0;
  int j = 2;
  int level = 0;
  char information[500] = "";
  char parsed[50][255];
  //printf("parsing this line : %s\n", toParse);
  char * token = strtok(toParse, " ");

  while(token != NULL){
    //printf("%s\n", token);
    strcpy(parsed[i], token);
    token = strtok(NULL, " ");
    i++;
  }
  if (i > 1){
    //printf("in the if state\n");
    while(strcmp(parsed[j], "\0") != 0){
        //printf("in the while\n");
      strcat(information, parsed[j]);
      strcpy(parsed[j], "\0");
      strcat(information, " ");
      j++;
      //printf("information in while %s\n", information);
    }
    parsed[j][strlen(parsed[j] - 2)] = '\0';
  }else{
    strcat(information, parsed[2]);
    strcpy(parsed[i], "\0");
  }
  level = atoi(parsed[0]);
  info.level = level;
  //strcpy(info.level, parsed[0]);
  if (parsed[1][0] == '@'){//when the address is in second place in GEDCOM line
    strcpy(info.info, parsed[1]);
    information[strlen(information) - 1] = '\0';
    //printf("printing info %s||\n", information);
    strcpy(info.tag, information);
  }else{
  information[strlen(information) - 1] = '\0';
  strcpy(info.tag, parsed[1]);
  strcpy(info.info, information);
  }
  //printf("\n[[%s || %s || %s]]\n", info.level, info.tag, info.info);

  return info;
}

char * printFunc(void * data){
  printf("data: %s\n", (char*)data);
  return NULL;
}

void deleteFunc(void * erase){

}

int compare(const void *first,const void *second){
  return 0;
}

Header * headParser(Info * record, int length){
  Header * head = malloc(sizeof(Header));
  int flag = 0;
  //char sour[249];
  float ver = 0;
  int i = 0;
  char * string = malloc(sizeof(char));

  List  other = initializeList(printField, deleteField, compareFields);
  Field * field;
  //insertBack(other, field);
  //char dadsd  = toString(other)
  printf("length of one record %d\n", length);

  printf("in head parser\n");
  for (int i = 0; i < length; i++){
    //printf("in head parser\n");
    if (strcmp(record[i].tag, "SOUR") == 0){ //name of the source
      printf("in SOUR\n");
      strcpy(head->source, record[i].info);
      i++;
      while(record[i].level != 1){//until next sub record
        if (strcmp(record[i].tag,"VERS") == 0){
          printf("in sOUR vers\n");
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field); // Version for SOUR
        }
        else if (strcmp(record[i].tag,"NAME") == 0){//name of SOUR
          printf("in NAME\n");
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        else if (strcmp(record[i].tag,"CORP") == 0){
          printf("in CIRO\n");
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
          if (strcmp(record[i].tag,"ADDR") == 0){
            field = createField(record[i].tag, record[i].info);
            insertBack(&other, field);
          }
        }
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "DEST") == 0){ // recieving system name
      printf("in DEST\n");
      field = createField(record[i].tag, record[i].info);
      //printf("Tag: %s, Value: %s", (char*)field->tag, (char*)field->value);
      insertBack(&other, field);
      //printf("Tag: %s, Value: %s", ((Field*)other.head->data)->tag, ((Field*)other.head->data)->value);
    }
    else if (strcmp(record[i].tag, "DATE") == 0){//Transmission date
      printf("in DATE\n");
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
      i++;
      while(record[i].level != 1){
        if (strcmp(record[i].tag, "TIME") == 0){
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "SUBM") == 0){
      printf("in SUBM\n");
      flag = 1;//means submitter exist
      //i++;
    }
    else if (strcmp(record[i].tag, "SUBN") == 0){
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
      //i++;
    }
    else if (strcmp(record[i].tag, "FILE") == 0){
    //  printf("in FILE\n");
      //printf("in struct <%s><%s>\n", record[i].tag, record[i].info);
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
      //printf("Tag: %s, Value: %s", ((Field*)other.head->next->next->data)->tag, ((Field*)other.head->next->next->data)->value);
      //i++;
    }
    else if (strcmp(record[i].tag, "COPR") == 0){
      printf("in CORP\n");
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
      //i++;
    }
    else if (strcmp(record[i].tag, "GEDC") == 0){
      printf("in GEDC\n");
      i++;
      while(record[i].level != 1){
        if (strcmp(record[i].tag, "VERS") == 0){//version of the GEDCOM file
          printf("in VERS GEDCM\n");
          ver = strtol(record[i].info, NULL, 0);
          head->gedcVersion = ver;
        }
        if (strcmp(record[i].tag, "FORM") == 0){
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "CHAR") == 0){
      printf("checking for char\n");
      if (strcmp(record[i].info, "ANSEL") == 0){
        head->encoding = ANSEL;
      }
      else if (strcmp(record[i].info, "UTF-8") == 0){
        head->encoding = UTF8;
      }
      else if (strcmp(record[i].info, "UNICODE") == 0){
        head->encoding = UNICODE;
      }
      else if (strcmp(record[i].info, "ASCII") == 0){
        head->encoding = ASCII;
      }
      if (i + 1 < length){
        i++;
        while(record[i].level != 1){
          if (strcmp(record[i].tag, "VERS") == 0){
            field = createField(record[i].tag, record[i].info);
            insertBack(&other, field);
          }
          i++;
        }
        i--;
      }
    }
    else if (strcmp(record[i].tag, "LANG") == 0){
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
    }
    else if (strcmp(record[i].tag, "PLAC") == 0){
      i++;
      while(record[i].level != 1){
        if (strcmp(record[i].tag, "FORM") == 0){
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "NOTE") == 0){
      /*i++;
      while(record[i].level < != NULL){
        if (strcmp(record[i].tag, "CONT") == 0){
          string = realloc(string, sizeof(char)*(strlen(record[i].info) + strlen(string)));
          strcat(string, record[i].info);
        }
        i++;
      }*/
    }
  }
  printf("\nSource: %s, GED VER: %f, Encoding: %d\n", head->source, head->gedcVersion, head->encoding);
  char * print = toString(other);
  puts(print);
  return head;
}

submitter * subParser(){
  submitter * sub = malloc(sizeof(submitter));
  pointer * tmep = malloc
  if (sub.tag == NAMe){
    s
  }
  temp->adrss = Wolverhampton
  temp->pointer = sub;
  
  return sub;
}
