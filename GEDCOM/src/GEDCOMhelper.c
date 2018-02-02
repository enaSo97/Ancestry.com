#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMmutilities.h"

void enumToString(ErrorCode error){
  if (error == OK){
    printf("OK\n");
  }
  else if(error == INV_FILE){
    printf("INV_FILE\n");
  }
  else if(error == INV_GEDCOM){
    printf("INV_GEDCOM\n");
  }
  else if(error ==  INV_HEADER){
    printf("INV_HEADER\n");
  }
  else if(error == INV_RECORD){
    printf("INV_RECORD\n");
  }
  else if(error == OTHER){
    printf("OTHER\n");
  }
}

/*******sets the information of the Event ********************************/
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
/*************************************************************************/

Field * createField(char tag[20], char value[255]){
  Field * create = calloc(1,sizeof(Field));

  create->tag = (char *)calloc(100,sizeof(char));
  strcpy(create->tag, tag);
  create->value = (char*)calloc(100,sizeof(char));
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

/*****************Helper Function for List of Pointers *****************/
char * printPointers(void * data){
  Pointer * point;
  char * string;
  int length;

  if (data == NULL){
    return NULL;
  }
  point = (Pointer*)data;

  length = strlen(point->addr) + strlen(point->type) + 500;
  string = (char*)calloc(length, sizeof(char));
  sprintf(string, "Type: %s, Address: %s\n", point->type, point->addr);
  /*if (strcmp(point->type, "CHIL") == 0){
    printf("it's children\n");
    strcat(string, printIndividual((Individual*)point->point));
  }*/

  return string;
}

void deletePointers(void * erase){
  Pointer * Delete;

  if (erase == NULL){
    return;
  }

  Delete = (Pointer*)erase;

  free(Delete->indiPoint);
  free(Delete->subPoint);
  //free(Delete->listPtr);
  free(Delete->stuff);
  free(Delete);
}

bool comparePointers(const void *first,const void *second){
  Pointer * compare1;
  Pointer * compare2;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Pointer*)first;
  compare2 = (Pointer*)second;

  if (strcmp(compare1->addr, compare2->addr) == 0){
    return true;
  }

  return false;
}

int dummyCompare(const void *first,const void *second){
  Pointer * compare1;
  Pointer * compare2;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Pointer*)first;
  compare2 = (Pointer*)second;

  return strcmp(compare1->addr, compare2->addr);
}

/*************************************************************************/

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

/** converting month of the year in to numbers ***/
int getMonth(char * getter){
  char month[12][5] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

  for (int i = 0; i < 12; i++){
    if (strcmp(month[i], getter)){
      return i + 1;
    }
  }
    return 0;
}
/*************************************************/

/******Validating the File ************************/
enum eCode validateFile(char* fileName){
  printf("in the function\n");
  //char ext[80];
  if (strlen(fileName) == 0){ // checking if the file argument is empty, file is invalid
    printf("empty file name\n");
    return INV_FILE;
  }

  char * end = strstr(fileName, ".ged");
  if (strcmp(end, ".ged") != 0){
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
/**************************************************/

/*bool validateTags(char * check){
  char tags[] = {"ABBR", "ADDR", "ADR1", "ADR2", "ADOP", "AFN", "AGE", "AGNC", "ALIA", "ANCE", "ANCI", "ANUL"
                  ,"ASSO", "AUTH", "BAPL", "BAPM", "BARM", "BASM", "BIRT", "BLES", "BURI", "CALN", "CAST", "CAUS", "CENS", "CHAN",
                  "CHAR", "CHIL", "CHR", "CHRA", "CITY", "CONC", "CONF", "CONL", "CONT", "COPR", "CORP", "CREM", "CTRY", "DATA", "DEAT",
                  "DESC", "DESI", "DEST", "DIV", "DIVF", "DSCR", "EDUC", "EMAI", "EMIG", "ENDL", "ENGA", "EVEN", "FACT", "FAM", "FAMC",
                  "FAMS", "FAMF", "FAX", "FCOM", "FILE", "FORM", "FONE", "GEDC", "GIVN", "GRAD", "HEAD", "HUSB", "IDNO", "IMMI", "INDI",
                  "LANG", "LATI", "LONG", "MAP", "MARB", "MARC", "MARL", "MARR", "MARS", "MEDI", "NAME", "NATI", "NCHI", "NICK", "NMR",
                  "NOTE", "NPFX", "NSFX", "OBJE", "OCCU", "ORDI", "ORDN", "PAGE", "PEDI", "PHON", "PLAC", "POST", "PROB", "PROP", "PUBL",
                  "QUAY", "RFFN", "RELA", "RELI", "REPO", "RESI", "RESN", "RFN", ""}
}
*/
/***********validating the GEDCOM line ************/
enum eCode validateRecord(Info * record, int length){
  //int length = fileLength(read);
  //int term;
  char convert[20];
  if (record[0].level != 0){
    return INV_RECORD;

  }
  for (int i = 0; i < length; i++){
    if (record[i].level < 0 || record[i].level > 99){
      return INV_RECORD;
    }
    else if(record[i].level != (record[i-1].level + 1)){
      return INV_RECORD;
    }
    else if(record[i].info[0] == '@'){
      if (strlen(record[i].info) > 22){
        return INV_RECORD;
      }
      /*else if (record[i].info[strlen(record[i]) - 1] != '@'){
        return INV_RECORD;
      }*/
    }
    /*itoa(record[i].level, convert, 10);
    if ((strlen(convert) + strlen(record[i].info) + strlen(record[i].tag)) > 255){
      return INV_RECORD;
    }*/
    else if (strlen(record[i].tag) > 32){
      return INV_RECORD;
    }
  }
  return OK;
}
/***************************************************/

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
  char** array = calloc(10000000, sizeof(char*));
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
  int j = 2;
  int level = 0;
  char information[500] = "";
  char parsed[100][255] = {""};
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
    //parsed[j][strlen(parsed[j])-2] = '\0';
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

Header * headParser(Info * record, int length, List * pointers, List * receiver){
  Header * head = malloc(sizeof(Header));
  //int flag = 0;
  //char sour[249];
  float ver = 0;
  //int i = 0;
  //char * string = malloc(sizeof(char));

  List  other = initializeList(printField, deleteField, compareFields);
  Pointer * point = calloc(1, sizeof(Pointer));
  Field * field;
  //Submitter sub;
  //insertBack(other, field);
  //char dadsd  = toString(other)
  printf("length of one record %d\n", length);

  printf("in head parser\n");
  for (int i = 0; i < length; i++){
    //printf("in head parser\n");
    if (strcmp(record[i].tag, "SOUR") == 0){ //name of the source
      strcpy(head->source, record[i].info);
      i++;
      while(record[i].level != 1){//until next sub record
        if (strcmp(record[i].tag,"VERS") == 0){
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field); // Version for SOUR
        }
        else if (strcmp(record[i].tag,"NAME") == 0){//name of SOUR
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        else if (strcmp(record[i].tag,"CORP") == 0){
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
      field = createField(record[i].tag, record[i].info);
      //printf("Tag: %s, Value: %s", (char*)field->tag, (char*)field->value);
      insertBack(&other, field);
      //printf("Tag: %s, Value: %s", ((Field*)other.head->data)->tag, ((Field*)other.head->data)->value);
    }
    else if (strcmp(record[i].tag, "DATE") == 0){//Transmission date
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
      strcpy(point->addr, record[i].info);
      strcpy(point->type, record[i].tag);
      point->subPoint = &head->submitter;
      insertBack(receiver, point);
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
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
      //i++;
    }
    else if (strcmp(record[i].tag, "GEDC") == 0){
      i++;
      while(record[i].level != 1){
        if (strcmp(record[i].tag, "VERS") == 0){//version of the GEDCOM file
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
  /*printf("\nSource: %s, GED VER: %f, Encoding: %d\n", head->source, head->gedcVersion, head->encoding);
  char * print = toString(other);
  puts(print);
  head->otherFields = other;*/
  return head;
}

Submitter * subParser(Info * record, int length, List * pointers, List * receiver){

  Submitter * sub = calloc(1,sizeof(Submitter));
  Pointer * temp;
  Field * field;
  List other = initializeList(printField, deleteField, compareFields);
  char string[5000] = "";

  printf("\nin sub parser\n");

  for (int i = 0; i < length; i++){
    if (strcmp(record[i].tag, "NAME") == 0){
      strcpy(sub->submitterName, record[i].info);
    }
    else if (strcmp(record[i].tag, "SUBM") == 0){
      temp = calloc(1, sizeof(Pointer));
      strcpy(temp->addr, record[0].info);
      strcpy(temp->type, record[0].tag);
      temp->subPoint = &sub;
      insertBack(pointers, temp);
    }
    else if (strcmp(record[i].tag, "ADDR") == 0){
      strcpy(sub->address, record[i].info);
      i++;
      while(record[i].level != 1){
        if (strcmp(record[i].tag, "CONT") == 0){
          strcpy(string, record[i].info);
        }
        else{
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        i++;
      }
      /*if (strlen(string) > 0){
        field = createField(record[i].tag, record[i].info);
        insertBack(&other, field);
      }*/
      i--;
    }
    else{
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
    }
  }
  if (strlen(string) > 0){
    field = createField("CONT", string);
    insertBack(&other, field);
  }
  sub->otherFields = other;
  /*if (record[0].info[0] == '@' && record[0].info[strlen(record[0].tag) - 1] == '@'){
    strcpy(temp->addr, record[0].info);
    strcpy(temp->type, record[0].tag);
    temp->point = (void*)sub;
  }
  insertBack(pointers, temp);*/

  //printf("\nName: %s, Address: %s\n", sub->submitterName, sub->address);
  char * print = toString(other);
  puts(print);

  return sub;
}

int validateIndividualEvent(char * check){
  char * events[] = {"BIRT", "CHR", "DEAT", "BURI", "CREM", "ADOP", "BAPM", "BARM", "BASM", "BLES", "CHRA",
                      "CONF", "FCOM", "ORDN", "NATU", "EMIG", "IMMI", "CENS", "PROB", "WILL", "GRAD", "RETI", "EVEN"};

  for (int i = 0; i < 23; i++){
    if (strcmp(check, events[i]) == 0){
      return 1;
    }
  }

  return -1;
}

Individual * parseIndividual(Info * record, int length, List * pointers, List*  receiver){
  Individual * person = calloc(1, sizeof(Individual));
  Pointer * temp;
  Field * field = calloc(1, sizeof(Field));
  List other = initializeList(printField, deleteField, compareFields);
  List events = initializeList(printEvent, deleteEvent, compareEvents);
  char string[50] = "";
  //char last[50] = "";
  char names[10][50];
  printf("in individual parser\n");

  person->surname = malloc(sizeof(char)*50);
  person->givenName = malloc(sizeof(char)*50);
  for (int i = 0; i < length; i++){
    int n = 0;
    //printf("\n<<%d||%s||%s>>\n", record[i].level, record[i].tag, record[i].info);

    if (strcmp(record[i].tag, "NAME") == 0){
      if (strlen(record[i].info) > 0){// if name exist
        char * personName = strtok(record[i].info, "/");
        while(personName != NULL){ //parses the full name
          strcpy(names[n], personName);
          personName = strtok(NULL, "/"); // saving parsed name in to temp 2d array
          n++;
        }
        for (int j = 0; j < n; j++){
          if (j == n - 1){
            names[j][strlen(names[j])-1] = '\0';
            strcpy(person->surname, names[j]);
          }else{
            names[j][strlen(names[j]) - 1] = '\0';
            strcat(string, names[j]);
          }
        }
        if (strlen(string) > 0){
          strcpy(person->givenName, string);
        }
      }
      i++;
      while(record[i].level != 1){
        field = createField(record[i].tag, record[i].info);
        insertBack(&other, field);
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "INDI") == 0){
      temp = calloc(1, sizeof(Pointer));
      strcpy(temp->addr, record[i].info);
      strcpy(temp->type, "INDI");
      temp->indiPoint = &person;
      //printf("\nchecking for indi pointer %s || %s || \n", temp->addr, temp->type);
      insertBack(pointers, temp);
      printf("printing address %p\n", (void*)&person);
      //printf("checking for pointer %s", (Pointer*)temp->indiPoint)
    }
    else if (validateIndividualEvent(record[i].tag) == 1){
      Event * event = calloc(1, sizeof(Event));
      strcpy(event->type, record[i].tag);
      i++;
      while(record[i].level != 1){
        if(strcmp(record[i].tag, "PLAC") == 0){
          event->place = calloc(500, sizeof(char));
          strcpy(event->place, record[i].info);
        }
        else if (strcmp(record[i].tag, "DATE") == 0){
          event->date = calloc(200,sizeof(char));
          strcpy(event->date, record[i].info);
        }else{
          field = createField(record[i].tag, record[i].info);
          insertBack(&other, field);
        }
        i++;
      }
      insertBack(&events, event);
      i--;
    }
    else if(strcmp(record[i].tag, "FAMC") == 0 || strcmp(record[i].tag, "FAMS") == 0){
      temp = calloc(1, sizeof(Pointer));
      strcpy(temp->type, record[i].tag);
      strcpy(temp->addr, record[i].info);
      temp->indiPoint = &person;
      insertBack(receiver, temp);
    }
    else{
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
    }
  }
  person->events = events;
  person->otherFields = other;
  //char * print = toString(events);
  //puts(print);
  //char * output = toString(other);
  //puts(output);


  return person;
}

int validateFamilyEvent(char * check){
  char * events[] = {"ANUL", "CENS", "DIV", "DIVF", "ENGA", "MARB", "MARC", "MARR", "MARL",
                    "MARS", "RESI", "EVEN"};

  for (int i = 0; i < 12; i++){
    if (strcmp(check, events[i]) == 0){
      return 1;
    }
  }

  return -1;
}

Family * parseFamily(Info * record, int length, List * pointers, List * receiver){
  Family * family = calloc(1, sizeof(Family));
  Individual * person;
  Pointer * temp = calloc(1, sizeof(Pointer));
  Field * field = calloc(1, sizeof(Field));
  List other = initializeList(printField, deleteField, compareFields);
  List children = initializeList(printIndividual, deleteIndividual, compareIndividuals);
  //List events = initializeList(printEvent, deleteEvent, compareEvents);
  //List kids = initializeList(printIndividual, deleteIndividual, compareIndividuals);

  strcpy(temp->addr, record[0].info);
  strcpy(temp->type, "FAM");
  temp->stuff = (void*)&family;
  insertBack(pointers, temp);

  for (int i = 1; i < length; i++){
    if (strcmp(record[i].tag, "HUSB") == 0){
      temp = calloc(1, sizeof(Pointer));
      person = calloc(1,sizeof(Individual));
      family->husband = person;
      strcpy(temp->addr, record[i].info);
      strcpy(temp->type, "HUSB");
      temp->indiPoint = &(family->husband);
      insertBack(receiver, temp);
    }
    else if (strcmp(record[i].tag, "WIFE") == 0)
    {
      temp = calloc(1, sizeof(Pointer));
      person = calloc(1,sizeof(Individual));
      family->wife = person;
      strcpy(temp->addr, record[i].info);
      strcpy(temp->type, "WIFE");
      temp->indiPoint = &(family->wife);
      insertBack(receiver, temp);
    }
    else if (validateFamilyEvent(record[i].tag) == 1){
      i++;
      while(record[i].level != 1){
        field = createField(record[i].tag, record[i].info);
        insertBack(&other, field);
        i++;
      }
      i--;
    }
    else if (strcmp(record[i].tag, "CHIL") == 0){
      temp = calloc(1, sizeof(Pointer));
      person = calloc(1,sizeof(Individual));
      strcpy(temp->addr, record[i].info);
      strcpy(temp->type, "CHIL");
      temp->listPtr = &(family->children);
      insertBack(receiver, temp);
      //insertBack(&children, person);
      //family->children = children;
    }
    else{
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
    }
  }
  family->otherFields = other;

  return family;
}

void linkerFunction(List * pointer, void * data){
  Node * temp = pointer->head;

  Pointer * set = NULL;

  Pointer * point = NULL;

  if (data == NULL){
    return ;
  }
  set = (Pointer*)calloc(1, sizeof(Pointer));
  strcpy(set->addr,((Pointer*)data)->addr);

  printf("%s // %s \n", set->addr, set->type);

  char * buff = toString(*pointer);
  //puts(buff);
  int i = 0;

  int length = getLength(*pointer);
  //printf("lenfth of poitner %d\n", length);
  //List * childrenPtr = set->listPtr;


  while(i < length){
    //point = (Pointer*)temp;
    char * string = returnString(((Pointer*)temp)->addr);
    printf("pointer %s || receiver %s||\n", ((Pointer*)temp->data)->addr, set->addr);
    if (strcmp(((Pointer*)temp->data)->addr, set->addr) == 0){
      if (strcmp(set->type, "CHIL") == 0){
        printf("it's child\n");
        insertBack(set->listPtr, ((Pointer*)temp->data)->indiPoint);
      }
      else if(strcmp(set->type, "SUBM") == 0){
        printf("it's submitter\n");
        set->subPoint = ((Pointer*)temp->data)->subPoint;
        printf("name: %s\n", ((Submitter*)set->subPoint)->submitterName);
        printf("set the submitter\n");
      }
      else if(strcmp(set->type, "HUSB") == 0 || strcmp(set->type, "WIFE") == 0){
        printf("it's wife or husband\n");
        set->indiPoint = ((Pointer*)temp->data)->indiPoint;
        printf("printint the addres %p\n", (void*)set->indiPoint);
        char* indi = printIndividual(&set->indiPoint);
        puts(indi);
      }
      else if (strcmp(set->type, "FAMS") == 0 || strcmp(set->type, "FAMC") == 0){

      }
    }
    temp = temp->next;
    i++;
  }
  printf("done the loop\n");
   // printf("\n-----in linker------\n");
   // char * child = toString(*childrenPtr);
   // puts(child);
   // printf("--------------------------\n");

}

char * returnString(char * data){
  char * str = calloc(50,sizeof(char));
  int i = 0;
  if (data == NULL){
    return NULL;
  }

  for (i = 0; i < strlen(data); i++){
    str[i] = data[i];
  }
  str[i] = '\0';

  return str;
}
