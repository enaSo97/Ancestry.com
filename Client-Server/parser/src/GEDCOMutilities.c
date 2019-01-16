#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"


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
  create->value = (char*)calloc(500,sizeof(char));
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
  /*Pointer * Delete;

  if (erase == NULL){
    return;
  }

  Delete = (Pointer*)erase;

  free(Delete->indiPoint);
  //free(Delete->listPtr);
  free(Delete);*/

}

bool comparePointers(const void *first,const void *second){
  /*Pointer * compare1;
  Pointer * compare2;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Pointer*)first;
  compare2 = (Pointer*)second;

  if (strcmp(compare1->addr, compare2->addr) == 0){
    return true;
  }

  return false;*/
  return true;
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
  //printf("in the function\n");
  //char ext[80];
  /*if (strlen(fileName) == 0){ // checking if the file argument is empty, file is invalid
    //printf("empty file name\n");
    return INV_FILE;
  }*/
  
  if (fileName == NULL){
	  return INV_FILE;
  }

  //char * end = strstr(fileName, ".ged");
  if (strstr(fileName, ".ged") == NULL){
    return INV_FILE;
  }

  FILE * file = fopen(fileName, "r");
  if (file == NULL){
    //printf("file is NULL, empty\n");
    return INV_FILE;
    //return NULL;
  }
  fclose(file);
  //ext = strrchr(fileName, '.');
  return OK;
}
enum eCode validateRecord(Info * record, int length){
  //int length = fileLength(read);
  //int term;
  //char convert[20];
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
  char** array = NULL;
  array = calloc(100000, sizeof(array));
  //char reader[1000];
  int i = 0;
  FILE * file = fopen(fileName, "r");
  int c;
  //int bump = 0;
  //printf("int the file reading\n");
  int j = 0;
  char * code = NULL;
  code =  calloc(10000, sizeof(char));
  while(1){
	  c = getc(file);
	  if (c == EOF){
		break;
	  }
	  code[j++] = (char)c;
  }
  
  char * token = strtok(code, "\n\r");

  while(token != NULL){
    array[i] = calloc(500, sizeof(char));
    strcpy(array[i], token);
    token = strtok(NULL, "\r\n");
    i++;
  }
  fclose(file);
  free(code);
  return array;
}

int fileLength(char ** array){
  int i = 0;
  while(array[i] != NULL){
    i++;
  }
  return i;
}

Info tockenInfo(char * toParse){ //parses the line of GEDCOM and saves into temporary info struct
  Info info;
  int i = 0;
  int j = 2;
  int level = 0;
  char information[1000] = "";
  char parsed[100][1000] = {""};
  info.strlen = strlen(toParse);
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
    strcpy(parsed[2], "\0");
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
    //printf("shit is here\n");
    strcpy(info.tag, parsed[1]);
    strcpy(info.info, information);
  }
  //printf("\n[[%d || %s || %s]]\n", info.level, info.tag, info.info);

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

enum eCode validateHeader(Info * record, int length){
	
	if (length > 2){
		return INV_HEADER;
	}
	
	/*for (int i = 0; i < length; i++){
		if (record[i])
	}*/
	return OK;
}

Header * headParser(Info * record, int length, List * pointers, List * receiver){
  Header * head = malloc(sizeof(Header));
  //int flag = 0;
  //char sour[249];
  float ver = 0;
  //int i = 0;
  //char * string = malloc(sizeof(char));

  List  other = initializeList(printField, deleteField, compareFields);
  //Pointer * point = calloc(1, sizeof(Pointer));
  Field * field;
  //Submitter sub;
  //insertBack(other, field);
  //char dadsd  = toString(other)
  //printf("length of one record %d\n", length);

  //printf("in head parser\n");
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
    /*else if (strcmp(record[i].tag, "SUBM") == 0){
      strcpy(point->addr, record[i].info);
      strcpy(point->type, record[i].tag);
      point->subPoint = head->submitter;
      insertBack(receiver, point);
      //i++;
    }*/
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
          ver = atof(record[i].info);
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
      //printf("checking for char\n");
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
  puts(print);*/
  head->otherFields = other;
  return head;
}

Submitter * subParser(Info * record, int length, List * pointers, List * receiver){

  Submitter * sub = calloc(1000,sizeof(Submitter));
  //Pointer * temp;
  Field * field;
  List other = initializeList(printField, deleteField, compareFields);
  char string[5000] = "";
  int addFlag = 0;
  //printf("\nin sub parser\n");

  for (int i = 0; i < length; i++){
    if (strcmp(record[i].tag, "NAME") == 0){
      strcpy(sub->submitterName, record[i].info);
    }
    /*else if (strcmp(record[i].tag, "SUBM") == 0){
      temp = calloc(1, sizeof(Pointer));
      strcpy(temp->addr, record[0].info);
      strcpy(temp->type, record[0].tag);
      temp->subPoint = sub;
      insertBack(pointers, temp);
    }*/
    else if (strcmp(record[i].tag, "ADDR") == 0){
	  addFlag = 1;
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
  if (addFlag == 0){
	  strcpy(sub->address, "");
  }
  if (strlen(string) > 0){
    field = createField("CONT", string);
    insertBack(&other, field);
  }
  sub->otherFields = other;
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

Individual * parseIndividual(Info * record, int length, List * pointers, List*  receiver, List * reference){
  Individual * person = calloc(1, sizeof(Individual));
  //Pointer * temp;
  Field * field /*= calloc(1, sizeof(Field))*/;
  List other = initializeList(printField, deleteField, compareFields);
  List events = initializeList(printEvent, deleteEvent, compareEvents);
  Reference * ref = calloc(1,sizeof(Reference));
  //char string[50] = "";
  char names[10][50] = {""};

  person->surname = calloc(50,sizeof(char));
  person->givenName = calloc(50,sizeof(char));
  for (int i = 0; i < length; i++){
    int n = 0;
    //int surFlag = 0;
    if (strcmp(record[i].tag, "NAME") == 0){
      if(record[i].info[0] == '/' && record[i].info[strlen(record[i].info) - 1] == '/'){
        char * personName = strtok(record[i].info, "/");
        while(personName != NULL){ //parses the full name
          //printf("when only surname |%s|\n", personName);
          strcpy(person->givenName, "");
          strcpy(person->surname, personName);
          personName = strtok(NULL, "/"); // saving parsed name in to temp 2d array
        }
        //printf("first |%s| last |%s|\n", person->givenName, person->surname);
      }else{
        char * personName = strtok(record[i].info, "/ ");
        while(personName != NULL){
          strcpy(names[n], personName);
          //printf("each name |%s|\n", names[n]);
          n++;
          personName = strtok(NULL, "/ ");
        }
        
        if (n == 1){//just the first name
			strcpy(person->givenName, names[0]);
		}else{
			strcpy(person->surname, names[n-1]);
			char firstName[50] = "";
			for (int j = 0; j < n -1; j++){
			  strcat(firstName, names[j]);
			  strcat(firstName, " ");
			}
			firstName[strlen(firstName) - 1] = '\0';
			strcpy(person->givenName, firstName);
		}
        //printf("first |%s| last |%s|\n", person->givenName, person->surname);
      }
    }
    
    //printf("first |%s| last |%s|\n", person->givenName, person->surname);
    if (strcmp(record[i].tag, "INDI") == 0){
      //ref = calloc(1, sizeof(Reference));
      strcpy(ref->indi, record[i].info);
      //field = createField(record[i].tag, record[i].info);
      //insertBack(&other, field);
      //insertBack(&GARBAGE_COLLECTOR,field);
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
      //ref = calloc(1, sizeof(Reference));
      if (strcmp(record[i].tag, "FAMC") == 0){
        strcpy(ref->FAMC, record[i].info);
      }
      if (strcmp(record[i].tag, "FAMS") == 0){
        strcpy(ref->FAMS, record[i].info);
      }
      //field = createField(record[i].tag, record[i].info);
      //insertBack(&other, field);
      //insertBack(&GARBAGE_COLLECTOR, field);
    }
    else if (strcmp(record[i].tag, "GIVN") == 0){
      //strcpy(person->givenName, record[i].info);
    }
    else if (strcmp(record[i].tag, "SURN") == 0){
      //strcpy(person->surname, record[i].info);
    }
    else if (strcmp(record[i].tag, "NAME") != 0){
      field = createField(record[i].tag, record[i].info);
      insertBack(&other, field);
    }
  }
  person->events = events;
  person->otherFields = other;
  ref->point = person;
  insertBack(reference, ref);
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

Family * parseFamily(Info * record, int length, List people, List * reference){
  Family * dummyFamily = calloc(1, sizeof(Family));
  dummyFamily->children = initializeList(&printIndividual, &dummyDelete, &compareIndividuals);
  char xref[32] = "";
  char kidRef[20][32] = {""};
  int k = 0;
  for (int i = 0; i < length; i++){
    if (strcmp(record[i].tag, "WIFE") == 0){
      strcpy(xref, record[i].info);
    }
  }
  //printf("\nwife ref %s\n", xref);
  Node * node = reference->head;
  while(node != NULL){
    Reference * ref = (Reference*)node->data;
    //puts(printReference(ref));
    if (strcmp(xref, ref->indi) == 0){
      //printf("wife found\n");
      dummyFamily->wife = ref->point;
      insertBack(&ref->point->families, dummyFamily);
    }
    node = node->next;
  }
  for (int i = 0; i < length; i++){
    if (strcmp(record[i].tag, "HUSB") == 0){
      strcpy(xref, record[i].info);
    }
  }
  Node * node1 = reference->head;
  while(node1 != NULL){
    Reference * ref = (Reference*)node1->data;
    if (strcmp(xref, ref->indi) == 0){
      //printf("husband foumd\n");
      dummyFamily->husband = ref->point;
      insertBack(&ref->point->families, dummyFamily);
    }
    node1 = node1->next;
  }

  k = 0;
  //printf("length of record %d\n", length);
  for (int i = 0; i < length; i++){
   //k = 0;
    if (strcmp(record[i].tag, "CHIL") == 0){
      //printf("found children %s\n", record[i].info);
      strcpy(kidRef[k], record[i].info);
      //printf("saving to 2d array %s\n", record[i].info);
      k++;
    }
  }

  for (int i = 0; i < k; i++){
    Node * node2 = reference->head;
    while(node2 != NULL){
      Reference * ref = (Reference*)node2->data;
      if (strcmp(kidRef[i], ref->indi) == 0){
        insertBack(&(dummyFamily->children), ref->point);
        insertBack(&(ref->point->families), dummyFamily);
      }
      node2 = node2->next;
    }
  }
  /*Node * node = people.head;//
  Individual * one;
  Node * field;
  Field * second;
  puts(toString(*reference));
  while(node != NULL){
    one = (Individual*)node->data;
    field = one->otherFields.head;
    while(field != NULL){
      second = (Field*)field->data;
      if(strcmp(xref, second->value) == 0){
        dummyFamily->wife = one;
        insertBack(&(one->families), dummyFamily);
        //free(dummyFamily);
        //insertBack(&FAMILY_GARBAGE, dummyFamily);
        break;
      }
      field = field->next;
    }
    node = node->next;
  }*/

  /*for (int i = 0; i < length; i++){
    if (strcmp(record[i].tag, "HUSB") == 0){
      strcpy(xref, record[i].info);
    }
  }
  Node * node2 = people.head;//
  Individual * one2;
  Node * field2;
  Field * second2;
  while(node2 != NULL){
    one2 = (Individual*)node2->data;
    field2 = one2->otherFields.head;
    while(field2 != NULL){
      second2 = (Field*)field2->data;
      if(strcmp(xref, second2->value) == 0){
        dummyFamily->husband = one2;
        insertBack(&(one2->families), dummyFamily);
        //insertBack(&FAMILY_GARBAGE, dummyFamily);
        break;
      }
      field2 = field2->next;
    }
    node2 = node2->next;
  }*/
    /*k = 0;

    //printf("length of record %d\n", length);
    for (int i = 0; i < length; i++){
      //k = 0;
      if (strcmp(record[i].tag, "CHIL") == 0){
        //printf("found children %s\n", record[i].info);
        strcpy(kidRef[k], record[i].info);
        //printf("saving to 2d array %s\n", record[i].info);
        k++;
      }
    }
    for (int v = 0; v < k; v++){
      Node * node3 = people.head;
      Individual * one3;
      Node * field3;
      Field * second3;
      while(node3 != NULL){
        one3 = (Individual*)node3->data;
        field3 = one3->otherFields.head;
        while(field3 != NULL){
          second3 = (Field*)field3->data;
          if(strcmp(kidRef[v], second3->value) == 0){
            
            insertBack(&(dummyFamily->children), one3);
            insertBack(&(one3->families), dummyFamily);
            //insertBack(&FAMILY_GARBAGE, dummyFamily);

          }
          field3 = field3->next;
        }
        node3 = node3->next;
      }
    }*/
    List events = initializeList(printEvent, deleteEvent, compareEvents);
    List other = initializeList(printField, deleteField, compareFields);
    Field * temp;
    for (int i = 0; i < length; i++){
      if (validateFamilyEvent(record[i].tag) == 1){
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
            temp = createField(record[i].tag, record[i].info);
            insertBack(&other, temp);
          }
          i++;
        }
        insertBack(&events, event);
        i--;
      }
    }
    //dummyFamily->otherFields = other;
  dummyFamily->events = events;
  strcpy(xref, "\0");
  //printf("-----------------------\n");
  //puts(printFamily(dummyFamily));
  //printf("------------------------\n");
  return dummyFamily;
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

List collectDescendents(Individual * person, List * listOfDescendents){
	if (person == NULL){
    return *listOfDescendents;
	}
  Node * fam = person->families.head;
  while(fam != NULL){

    Node * e = ((Family*)fam->data)->children.head;
    if (((Family*)fam->data)->husband == person || ((Family*)fam->data)->wife == person){//it is husban of the family
      // level++;
      while(e != NULL){
        //level++;
        
        Individual * send = ((Individual*)e->data);
        Individual * copy = NULL;
        copy = copyIndividual(send);
        e = e->next;
        insertBack(listOfDescendents, copy);
        *listOfDescendents = collectDescendents(send, listOfDescendents);
      }
    }
    fam = fam->next;
  }
  return *listOfDescendents;
}

bool comparePerson(const void* first, const void* second){
  Individual * compare1 = NULL;
  Individual * compare2 = NULL;
  //printf("comparing individuals\n");
  if (first == NULL || second == NULL){
    return false;
  }
  // compare1->givenName = "";
  // compare1->surname = "";
  // compare2->givenName = "";
  // compare2->surname = "";

  compare1 = (Individual*)first;
  compare2 = (Individual*)second;
  if (compare1 != NULL && compare2 != NULL){
    if(strcmp(compare1->givenName, compare2->givenName) == 0){//when the given name match
     if (strcmp(compare1->surname, compare2->surname) == 0){//when the last name match
       return true;
       /*if (compare1->families.length == compare2->families.length){//when the number of family match
         if (compare1->events.length == compare2->events.length){//when the number of events match
           return true;
         }//end if 
       }//end if */
     }//end if 
   }//end if 
  }
  
  //  if(strcmp(compare1->givenName, compare2->givenName) == 0){//when the given name match
  //    if (strcmp(compare1->surname, compare2->surname) == 0){//when the last name match
  //      if (compare1->families.length == compare2->families.length){//when the number of family match
  //        if (compare1->events.length == compare2->events.length){//when the number of events match
  //          return true;
  //        }//end if 
  //      }//end if 
  //    }//end if 
  //  }//end if 

   return false;
}//end customCompare

List DescendentWrapper(Individual * person, List * lists, int var){
  if (person == NULL){
    return *lists;
	}
  //EachGen * thing = calloc(1, sizeof(EachGen));
  Node * fam = person->families.head;
  while(fam != NULL){
    Node * c = ((Family*)fam->data)->children.head;
    if (((Family*)fam->data)->husband == person || ((Family*)fam->data)->wife == person){//it is husban of the family
     // Node * c = ((Family*)fam->data)->children.head;
      while(c != NULL){
        var++;
        Individual * child = ((Individual*)c->data);
        c = c->next;
        EachGen * thing = calloc(1, sizeof(EachGen));
        thing->indi = copyIndividual(child);
        thing->level = var;
        insertBack(lists, thing);
        *lists = DescendentWrapper(child, lists, var);
        var--;
      }
    }
    fam = fam->next;
  }
  
  return *lists;
}

List AncestorWrapper(Individual * person, List * list, int var){
  if (person == NULL){
    return *list;
  }
  Node * fam = person->families.head;
  while(fam != NULL){
    Family * family = (Family*)fam->data;
    Node * node = family->children.head;
    while(node != NULL){
      //var++;
      Individual * child = (Individual*)node->data;
      if (child == person){//it is child of the family
        if (family->husband != NULL){
          var++;
          EachGen * dad = calloc(1, sizeof(EachGen));
          dad->level = var;
          dad->indi = copyIndividual(family->husband);
          insertBack(list, dad);
          *list = AncestorWrapper(family->husband, list, var);
          var--;
        }
        if (family->wife != NULL){
          var++;
          EachGen * mom = calloc(1, sizeof(EachGen));
          mom->level = var;
          mom->indi = copyIndividual(family->wife);
          insertBack(list, mom);
          *list = AncestorWrapper(family->wife, list, var);
          var--;
        }
      }
      node = node->next;
    }
    fam = fam->next;
  }
  return *list;
}
void deleteStruct(void * toBeDeleted){
  EachGen * Delete;
  if (toBeDeleted == NULL){
    return;
  }
  Delete = (EachGen*)toBeDeleted;
  //free(Delete->indi);
  free(Delete);
}

char * printStruct(void * print){
  EachGen * str;
  if (print == NULL){
    return NULL;
  }
  str = (EachGen*)print;
  char * string = NULL;
  int len = 0;
  len = strlen(str->indi->givenName) + strlen(str->indi->surname);
  string = calloc(len + 50, sizeof(char));
  sprintf(string, "level: %d\n", str->level);
  strcat(string, "givenName: ");
  strcat(string, str->indi->givenName);
  strcat(string, "\nsurname: ");
  strcat(string, str->indi->surname);
  strcat(string, "\n");
  return string;
}
void deleteIndiReference(void * toBeDeleted){
  IndividualRef * Delete;
  if (toBeDeleted == NULL){
    return;
  }
  Delete = (IndividualRef*)toBeDeleted;
  clearList(&Delete->famcRef);
  clearList(&Delete->famsRef);
  //ree(Delete->point);
  free(Delete);
}
void deleteFamReference(void * toBeDeleted){
  FamilyRef * Delete;
  if (toBeDeleted == NULL){
    return;
  }
  Delete = (FamilyRef*)toBeDeleted;
  clearList(&Delete->children);
  free(Delete);
}
void dummyDelete(void * toBeDeleted){
  //dummy delete function 
}

Individual * copyIndividual(Individual * toCopy){
  if (toCopy == NULL){
    return NULL;
  }
  int len = 0;
  Individual * person = calloc(1, sizeof(Individual));
  if (toCopy->givenName != NULL){
    len = strlen(toCopy->givenName);
    person->givenName = calloc(len + 50,sizeof(char));
    strcpy(person->givenName, toCopy->givenName);
  }
  if (toCopy->surname != NULL){
    len = strlen(toCopy->surname);
    person->surname = calloc(len + 50,sizeof(char));
    strcpy(person->surname, toCopy->surname);
  }
  person->events = initializeList(printEvent, dummyDelete, compareEvents);
  //person->events = toCopy->events;
  person->otherFields = initializeList(printField, dummyDelete, compareEvents);
  //person->otherFields = toCopy->otherFields;
  person->families = initializeList(printFamily, dummyDelete, compareFamilies);
  //person->families = toCopy->families;
  return person;
}

void deleteReferenceList(void * toBeDeleted){
  Reference * Delete;
  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Reference*)toBeDeleted;
  free(Delete);
}

char * printReference(void * toBePrinted){
  Reference * print;
  char * string = calloc(100, sizeof(char));
  if (toBePrinted == NULL){
    strcpy(string, "");
    return string;
  }
  print = (Reference*)toBePrinted;

  sprintf(string, "indi %s\n", print->indi);
  char temp[30] = "";
  char temp2[30] = "";
  if (print->FAMS == NULL){
    strcpy(temp, "FAMS: \n");
    strcat(string, temp);
  }
  if (print->FAMS != NULL){
    sprintf(temp, "FAMS: %s\n", print->FAMS);
    strcat(string, temp);
  }
  if (print->FAMC == NULL){
    strcpy(temp2, "FAMC: \n");
    strcat(string, temp2);
  }
  if (print->FAMC != NULL){
    sprintf(temp2, "FAMC: %s\n", print->FAMC);
    strcat(string, temp2);
  }
  return string;
}

void deleteGenStruct(void * toBeDeleted){
	Generation * Delete;
	if (toBeDeleted == NULL){
		return ;
	}
	Delete = (Generation*)toBeDeleted;
	
	free(Delete);
}


void deleteFinalList(void * toBeDeleted){
  List * Delete;
  if (toBeDeleted == NULL){
    return ;
  }
  Delete = (List*)toBeDeleted;
  Node * current = Delete->head;
  while(current != NULL){
    List * indi = (List*)current->data;
    clearList(indi);
    current = current->next;
  }
}
char * printListDescendents(void * toBePrinted){
  char * string = NULL;
  if (toBePrinted == NULL){
    return string;
  }
  List * print = (List*)toBePrinted;
  string = toString(*print);
  return string;
}

ErrorCode validateField(List otherFields){
	Node * node = otherFields.head;
	while(node != NULL){
		Field * field = (Field*)node->data;
		//printf("field fpr\n");
		if (field == NULL){
			return INV_RECORD;
		}
		else if (field != NULL){
			if (strlen(field->value) > 200){ //the field value is greather than 200
				return INV_RECORD;
			}
			else if (strlen(field->tag) > 200){
				return INV_RECORD;
			}
		}
		node = node->next;
	}
	return OK;
}

ErrorCode validateEvent(List events){
	//ErrorCode error = OK;
	if (events.length > 0){
		Node * node = events.head;
		for (int i = 0; i < events.length; i++){
			//printf("event for\n");
			Event * event = (Event*)node->data;
			if (event == NULL){
				return INV_RECORD;
			}
			else if (event != NULL){
				if (event->place != NULL){
					if(strlen(event->place) > 200){
						return INV_RECORD;
					}
				}
				else if (event->date != NULL){
					if(strlen(event->date) > 200){
						return INV_RECORD;
					}
				}
				else if (event->type != NULL){
					if (strlen(event->type) > 5){
						return INV_RECORD;
					}
				}
			}
			Node * event_other = event->otherFields.head;
			if (event->otherFields.length > 0){
				List * list = (List*)event_other->data;
				if(validateField(*list) == INV_RECORD){
					return INV_RECORD;
				}
				event_other = event_other->next;
			}
			node = node->next;
		}
	}
	return OK;
}

