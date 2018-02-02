#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GEDCOMparser.h"
#include "GEDCOMmutilities.h"
#include "LinkedListAPI.h"


//***************************************** GEDCOOM object functions *****************************************

/** Function to create a GEDCOM object based on the contents of an GEDCOM file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ics extension.
 File represented by this name must exist and must be readable.
 *@post Either:
 A valid GEDCOM has been created, its address was stored in the variable obj, and OK was returned
 or
 An error occurred, the GEDCOM was not created, all temporary memory was freed, obj was set to NULL, and the
 appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the GEDCOM
 *@param fileName - a string containing the name of the GEDCOM file
 *@param a double pointer to a GEDCOMobject struct that needs to be allocated
 **/
GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){
  GEDCOMerror errorCheck;
  //if(DEBUG)printf("int the create\n");
  char** read; // storing the each line of GEDCOM file in double pointer array
  int length;
  //int recLength = 0;
  Info * info;
  Info * record;
  //int flag = 0;
  //char ** stuff;
  int k =0;
  List pointers = initializeList(&printPointers, &deletePointers, &dummyCompare);

  List receiver = initializeList(&printPointers, &deletePointers, &dummyCompare);

  List people = initializeList(printIndividual, deleteIndividual, compareIndividuals);

  List family = initializeList(printFamily, deleteFamily, compareFamilies);

  Individual * dummyPerson = calloc(1, sizeof(Individual));

  Family * dummyFamily = calloc(1, sizeof(Family));

  GEDCOMobject * object = malloc(sizeof(GEDCOMobject));
  //*obj = malloc(sizeof(GEDCOMobject));

  read = fileReader(fileName);
  length = fileLength(read);
  info = calloc(1000000, sizeof(Info));
  record = calloc(1000000, sizeof(Info));

  if (validateFile(fileName) == INV_FILE){
    errorCheck = setType(INV_FILE, -1);
    obj = NULL;
  }
  /**
  ** this function tockenizes the line of GEDCOM line and put it into information struct
  **/
  for (int i = 0; i < length; i++){
    info[i] = tockenInfo(read[i]);
    info[i].line = i + 1;
  }
  for (int i = 0; i < length; i++){
    //printf("finding for record <%d>\n", i);
    int j = 1;
    //printf("tracking i %d\n", i);
    if (info[i].level == 0){
      k = 0;
      record[k].level = info[i].level;
      strcpy(record[k].tag, info[i].tag);
      strcpy(record[k].info, info[i].info);
      //printf("\nnew ||%s||%s||%s||\n", record[k].level, record[k].tag, record[k].info);
      k++;
      i++;
      j++;
      while(info[i].level != 0){
        //printf("info : %s i <%d>\n", info[i].info, i);
        //record = realloc(record, sizeof(Info) * j);
        record[k].level = info[i].level;
        strcpy(record[k].tag, info[i].tag);
        strcpy(record[k].info, info[i].info);
        k++;
        i++;
        j++;
      }
      //printf("record 0 ||%d||%s||%s||\n", record[0].level, record[0].tag, record[0].info);
      if (strcmp(record[0].tag, "HEAD") == 0){
        object->header = headParser(record, k, &pointers, &receiver);
      }
      else if (strcmp(record[0].tag, "SUBM") == 0){
        object->submitter = subParser(record, k, &pointers, &receiver);
        printf("passed submitter\n");
      }
      else if (strcmp(record[0].tag, "INDI") == 0){
        //printf("before parsing indi\n");
        dummyPerson = parseIndividual(record, k, &pointers, &receiver);
        insertBack(&people, dummyPerson);
        //printf("indi passed\n");
      }
      else if (strcmp(record[0].tag, "FAM") == 0){
        dummyFamily = parseFamily(record, k, &pointers, &receiver);
        insertBack(&family, dummyFamily);
        printf("passed fmailu\n");
      }
    }
    i--;
    //printf("<<<i : %d>>>\n", i);
    //i = k;
  }
  Node * receive = receiver.head;
  while(receive->next != NULL){
    printf("linker while loop\n");
    linkerFunction(&pointers, receive->data);
    receive = receive->next;
  }
  length--;
  /*
  printf("\nprinting individual\n");
  char * indi = toString(people);
  puts(indi);
  printf("*********************************************\n");*/

  char * print1 = toString(pointers);
  char * print2 = toString(receiver);
  printf("******pointers**************************\n");
  puts(print1);
  printf("\n******receivers**********************\n");
  puts(print2);
  printf("*******************************************");

  printf("\n--------Individual -------------\n");
//  char * indi = toString(family);
  //puts(indi);
  printf("---------------------------------\n");

  for (int i = 0; i < 10000000; i++){ // freeing the allocated memory after done parsing the file
    //free(info[i]); // freeing the allocated information struct
    free(read[i]);//freeing the allocated strings
  }
  free(read);
  free(info);
  free(record);

  return errorCheck;
}


/** Function to create a string representation of a GEDCOMobject.
 *@pre GEDCOMobject object exists, is not null, and is valid
 *@post GEDCOMobject has not been modified in any way, and a string representing the GEDCOM contents has been created
 *@return a string contaning a humanly readable representation of a GEDCOMobject
 *@param obj - a pointer to a GEDCOMobject struct
 **/
char* printGEDCOM(const GEDCOMobject* obj);


/** Function to delete all GEDCOM object content and free all the memory.
 *@pre GEDCOM object exists, is not null, and has not been freed
 *@post GEDCOM object had been freed
 *@return none
 *@param obj - a pointer to a GEDCOMobject struct
 **/
void deleteGEDCOM(GEDCOMobject* obj);


/** Function to "convert" the GEDCOMerror into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code
 *@param err - an error struct
 **/
char* printError(GEDCOMerror err);

/** Function that searches for an individual in the list using a comparator function.
 * If an individual is found, a pointer to the Individual record
 * Returns NULL if the individual is not found.
 *@pre GEDCOM object exists,is not NULL, and is valid.  Comparator function has been provided.
 *@post GEDCOM object remains unchanged.
 *@return The Individual record associated with the person that matches the search criteria.  If the Individual record is not found, return NULL.
 *If multiple records match the search criteria, return the first one.
 *@param familyRecord - a pointer to a GEDCOMobject struct
 *@param compare - a pointer to comparator fuction for customizing the search
 *@param person - a pointer to search data, which contains seach criteria
 *Note: while the arguments of compare() and person are all void, it is assumed that records they point to are
 *      all of the same type - just like arguments to the compare() function in the List struct
 **/
Individual* findPerson(const GEDCOMobject* failyRecord, bool (*compare)(const void* first, const void* second), const void* person);


/** Function to return a list of all descendants of an individual in a GEDCOM
 *@pre GEDCOM object exists, is not null, and is valid
 *@post GEDCOM object has not been modified in any way, and a list of descendants has been created
 *@return a list of descendants.  The list may be empty.  All list members must be of type Individual, and can appear in any order.
 *All list members must be COPIES of the Individual records in the GEDCOM file.  If the returned list is freed, the original GEDCOM
 *must remain unaffected.
 *@param failyRecord - a pointer to a GEDCOMobject struct
 *@param person - the Individual record whose descendants we want
 **/
List getDescendants(const GEDCOMobject* failyRecord, const Individual* person);


//************************************************************************************************************

//****************************************** List helper functions *******************************************
void deleteEvent(void* toBeDeleted){
  Event * Delete;

  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Event*)toBeDeleted;



  free(Delete->date);
  free(Delete->place);
  free(Delete);

}
/**
**return 0 when they are same
**return 1 when second first > second
**return -1 when
*/
int compareEvents(const void* first,const void* second){
  Event * A = (Event*)first;
  Event * B = (Event*)second;
  int result;
  /**************/
  int date1 = 0;
  char month1[5];
  int year1 = 0;
  /**************/
  /**************/
  int date2 = 0;
  char month2[5];
  int year2 = 0;
  /**************/
  if (first == NULL || second == NULL){
    return 0;
  }
  toLower(A->date);
  sscanf((char*)A->date, "%d %s %d", &date1, month1, &year1);
  sscanf((char*)B->date, "%d %s %d", &date2, month2, &year2);

  if (year1 > year2){ // clearly means that year1 is greater than year2
    result = 1;
  }
  if (year1 < year2){
    result = -1;
  }
  else if (year1 == year2){// when both event has the same year than we have to compare month
    if (month1 > month2){
      result = 1;
    }
    else if (month1 < month2){
      result = -1;
    }
    else if (month1 == month2){ //when both event have same month and same year we have to compare dates
      if (date1 > date2){
        result = 1;
      }else if (date1 < date2){
        result = -1;
      }else if (date1 == date2){
        result = 0; // when both events have exactly same dates
      }
      }
    }
    return result;
}
char* printEvent(void* toBePrinted){ //type date place other fields
  char* chrTemp;
  Event* temp;
  int length;
  if (toBePrinted == NULL){
    return NULL;
  }

  temp = (Event*)toBePrinted;

  if (temp->place == NULL){
    temp->place = calloc(1, sizeof(char));
    strcpy(temp->place, "\0");
    //printf("check if null\n");
  }
  if (temp->date == NULL){
    temp->date = calloc(1, sizeof(char));
    strcpy(temp->date, "\0");
    //printf("check if null\n");
  }


  //printf("\ntype: %s, date: %s, place: %s\n", temp->type, temp->date, temp->place);
  //char * print = toString(temp->otherFields);
  //printf("%s\n\n",print);

  //length = strlen(temp->type) + strlen(temp->date) + strlen(temp->place) + 300;
  chrTemp = (char*)calloc(1000, sizeof(char));
  sprintf(chrTemp, "type: %s, date: %s, place: %s\n", temp->type, temp->date, temp->place);
  if (temp->otherFields.head != NULL){
    //printf("when it is not null\n");
    strcat(chrTemp, toString(temp->otherFields));
  }
  //printf("right before returning print events\n");
  return chrTemp;
}

void deleteIndividual (void* toBeDeleted){
  Individual * Delete;

  if (toBeDeleted == NULL){
    return;
  }

  Delete = (Individual*)toBeDeleted;

  free(Delete->givenName);
  free(Delete->surname);
  free(Delete);
}

int compareIndividuals(const void* first,const void* second){
  Individual * compare1;
  Individual * compare2;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Individual*)first;
  compare2 = (Individual*)second;

  return strcmp(compare1->surname, compare2->surname);
}

char* printIndividual(void* toBePrinted){
  char* string;
  char * events;
  char * other;
  Individual* print;
  if (toBePrinted == NULL){
    return NULL;
  }
  print = (Individual*)toBePrinted;
  if (print->givenName == NULL){
    print->givenName = calloc(1, sizeof(char));
    strcpy(print->givenName, "\0");
    printf("check if null\n");
  }
  if (print->surname == NULL){
    print->surname = calloc(1, sizeof(char));
    strcpy(print->surname, "\0");
    printf("check if null\n");
  }


  //length = strlen(print->givenName) + strlen(print->surname) + 400;
  string = (char*)calloc(1000, sizeof(char));
  printf("in print individual\n");
  sprintf(string, "FirstName: %s, LastName: %s", print->givenName, print->surname);
  events = toString(print->events);
  other = toString(print->otherFields);
  puts(events);
  puts(other);
  if (print->events != NULL){
    strcat(string, toString(print->events));
  }
  //strcat(string, toString(print->events));
  strcat(string, toString(print->families));
  strcat(string, toString(print->otherFields));
  return string;
}

void deleteFamily(void* toBeDeleted){
  Family * Delete;

  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Family*)toBeDeleted;
  deleteIndividual(Delete->wife);
  deleteIndividual(Delete->husband);
  free(Delete);
}
int compareFamilies(const void* first,const void* second){
  Family * compare1 = (Family*)first;
  Family * compare2 = (Family*)second;

  int count2;

  return 0;
}
char* printFamily(void* toBePrinted){
  char * string;
  char * toadd;
  Family * print;

  if (toBePrinted == NULL){
    return NULL;
  }
  string = calloc(10000, sizeof(char));
  print = (Family*)toBePrinted;
  string = printIndividual(print->wife);
  //string = (char*)realloc(string, sizeof(char) * 100);// reallocating memory for next elements that;s in the struct
  strcat(string, printIndividual(print->husband));
  strcat(string, toString(print->children));
  strcat(string, toString(print->otherFields));

  return string;
}

void deleteField(void* toBeDeleted){
  Field * Delete;

  if (toBeDeleted == NULL){
    return;
  }

  Delete = (Field*)toBeDeleted;

  free(Delete->tag);
  free(Delete->value);
  free(Delete);
}
int compareFields(const void* first,const void* second){
  //Field * compare1 = (Field*)first;
  //Field * compare2 = (Field*)second;
  return 0;
}
char* printField(void* toBePrinted){
  char* string;
  Field * print;
  int length;
  if(toBePrinted == NULL){
    return NULL;
  }
  print = (Field*)toBePrinted;
  //printf("Tag: %s, Value: %s\n", (char*)print->tag, (char*)print->value);
  length = strlen(print->tag) + strlen(print->value) + 100;
  string = (char*)calloc(length, sizeof(char));
  sprintf(string, "Tag: %s, Value: %s\n", print->tag, print->value);
  return string;
}
//************************************************************************************************************
