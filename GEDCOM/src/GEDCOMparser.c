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
  char** read = fileReader(fileName); // storing the each line of GEDCOM file in double pointer array
  int length;
  length = fileLength(read);
  int recLength = 0;
  Info * info;
  Info * record;
  int flag = 0;
  char ** stuff;
  int k =0;
  info = calloc((length - 1), sizeof(Info));
  GEDCOMobject * object = malloc(sizeof(GEDCOMobject));
  //*obj = malloc(sizeof(GEDCOMobject));

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
    /*if (strcmp(info[i].level, "0") == 0){ // it means it is a new record
      flag = 0;
    }else{
      flag++;
    }*/
    //printf("struct info <%s><%s><%s>\n", info[i].level, info[i].tag, info[i].info);
  }
  for (int i = 0; i < 45; i++){
    int j = 1;
    //printf("tracking i %d\n", i);
    if (info[i].level == 0){
      k = 0;
      record = realloc(record, sizeof(Info) * j);
      record[k].level = info[i].level;
      strcpy(record[k].tag, info[i].tag);
      strcpy(record[k].info, info[i].info);
      //printf("\nnew ||%s||%s||%s||\n", record[k].level, record[k].tag, record[k].info);
      k++;
      i++;
      j++;
      while(info[i].level != 0){
        record = realloc(record, sizeof(Info) * j);
        record[k].level = info[i].level;
        strcpy(record[k].tag, info[i].tag);
        strcpy(record[k].info, info[i].info);
        //printf("\nfollowing ||%s||%s||%s||\n", record[k].level, record[k].tag, record[k].info);
        k++;
        i++;
        j++;
      }
      //printf("record 0 ||%d||%s||%s||\n", record[0].level, record[0].tag, record[0].info);
      if (strcmp(record[0].tag, "HEAD") == 0){
        printf("Head found\n");
        object->header = headParser(record, (k));
      }

    }
    i--;
    //i = k;
  }
  length--;
  for (int i = 0; i < length; i++){ // freeing the allocated memory after done parsing the file
    //free(info[i]); // freeing the allocated information struct
    free(read[i]);//freeing the allocated strings
  }
  free(read);
  free(info);
  //free(record);

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

  length = strlen(temp->type) + strlen(temp->date) + strlen(temp->place) + 40;
  chrTemp = (char*)calloc(length, sizeof(char));
  sprintf(chrTemp, "type: %s, date: %s, place: %s\n", temp->type, temp->date, temp->place);
  strcat(chrTemp, toString(temp->otherFields));

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
  Individual* print;
  int length;

  if (toBePrinted == NULL){
    return NULL;
  }

  print = (Individual*)toBePrinted;
  length = strlen(print->givenName) + strlen(print->surname) + 150;
  string = (char*)calloc(length, sizeof(char));

  sprintf(string, "FirstName: %s, LastName: %s", print->givenName, print->surname);
  strcat(string, toString(print->events));
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
/*int compareFamilies(const void* first,const void* second){
  Family * compare1 = (Family*)first;
  Fmaily * compare2 = (Family*)second;

  int count1 = getLength(compare1->);
  int count2;

  return 0;
}*/
char* printFamily(void* toBePrinted){
  char * string;
  char * toadd;
  Family * print;

  if (toBePrinted == NULL){
    return NULL;
  }

  print = (Family*)toBePrinted;
  string = printIndividual(print->wife);
  string = (char*)realloc(string, sizeof(char) * 100);// reallocating memory for next elements that;s in the struct
  toadd = printIndividual(print->husband);
  strcat(string, toadd);
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
  printf("in the print field\n");
  if(toBePrinted == NULL){
    printf("it is null\n");
    return NULL;
  }
  printf("set to null\n");
  print = (Field*)toBePrinted;
  printf("Tag: %s, Value: %s", (char*)print->tag, (char*)print->value);
  length = strlen(print->tag) + strlen(print->value) + 100;
  string = (char*)calloc(length, sizeof(char));
  /*printf("calloc length!!\n");
  sprintf(string, "Tag: %s, Value: %s", print->tag, print->value);
  printf("sprint!!!!!!!!!!\n");*/
  return string;
}
//************************************************************************************************************
