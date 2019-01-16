#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
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
  char** read; // storing the each line of GEDCOM file in double pointer array
  int length;
  //int recLength = 0;
  Info * info;
  Info * record;
  int subFlag = 0;
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
  
  if (validateFile(fileName) == INV_FILE){
    errorCheck = setType(INV_FILE, -1);
    obj = NULL;
    return errorCheck;
  }
  
  //*obj = malloc(sizeof(GEDCOMobject));
  printf("\nparsing this file |%s|\n", fileName);
  read = fileReader(fileName);
  printf("finding long ass line || %s\n", read[2]);
  length = fileLength(read);
  info = calloc(1000000, sizeof(Info));
  record = calloc(1000000, sizeof(Info));

  /**
  ** this function tockenizes the line of GEDCOM line and put it into information struct
  **/
  printf("\n after validateing parsing this file |%s|\n", fileName);
  for (int i = 0; i < length; i++){
    info[i] = tockenInfo(read[i]);
    info[i].line = i + 1;
  }
  printf("fitst one %d\n", info[length-1].level);
  printf("second one %s\n", info[length-1].tag);
  printf("third one %s\n", info[length-1].info);
  printf("last line of file %d || %s || %s || %d\n", info[length-1].level, info[length-1].tag, info[length-1].info, length-1);
  if (strcmp(info[length-1].tag, "TRLR") != 0){
	  printf("trlr is nor there\n");
	  errorCheck.type = INV_GEDCOM;
	  errorCheck.line = -1;
	  return errorCheck;
  }
  //printf(" afte tokenizing parsing this file |%s|\n", fileName);
  
  /*if (strcmp(info[length-1].tag, "TRLR") != 0){
	  errorCheck.type = INV_GEDCOM;
	  errorCheck.line = info[length-1].line;
	  return errorCheck;
  }*/
  
  int i = 0;
  while(i < length){
	  if (i == 0){//very first record from the file
		  //must be Header
		  if (strcmp(info[i].tag, "HEAD") != 0){
			  errorCheck.type = INV_GEDCOM;
			  errorCheck.line = -1;
			  return errorCheck;
		  }
	  }
	  if (i == length - 1){
		  if (strcmp(info[i].tag, "TRLR") != 0){
			  errorCheck.type = INV_GEDCOM;
			  errorCheck.line = info[i].line;
			  return errorCheck;
		  }
	  }
    if (info[i].level == 0){
      k = 0;
      record[k].level = info[i].level;//saving the first line of record into temporary record struct
      record[k].line = info[i].line;
      strcpy(record[k].tag, info[i].tag);
      strcpy(record[k++].info, info[i++].info);
      
      while(info[i].level != 0){ //saving the rest of the record until it reaches the next new record which is next 0
        record[k].level = info[i].level;
        record[i].line = info[i].line;
        strcpy(record[k].tag, info[i].tag);
        strcpy(record[k++].info, info[i++].info);
      }
      if (strcmp(record[0].tag, "HEAD") == 0){
		/***********************error checkin for header ****************************************************************/
		if (strcmp(record[1].tag, "SOUR") != 0){//checking for missing SOURCE
			errorCheck.type = INV_HEADER;
			errorCheck.line = 1;
			return errorCheck;
		}
		else if (strcmp(record[2].tag, "VERS") != 0 && strcmp(record[3].tag, "VERS") != 0) {
			errorCheck.type = INV_HEADER;
			errorCheck.line = 1;
			return errorCheck;
		}
		if (strcmp(record[k -2].tag, "CHAR") != 0){//checking for missing encoding
			errorCheck.type = INV_HEADER;
			errorCheck.line = k;
			return errorCheck;
		}
		if (strcmp(record[k -1].tag, "SUBM") != 0){//checking for missing submitter 
			errorCheck.type = INV_HEADER;
			errorCheck.line = k;
			return errorCheck;
		}
		/**********************error Checking for Header End*****************************************************************/
		for (int m = 0; m < k; m++){//this for loop is to validate the record
			//printf("cheking for record\n");
			if (strlen(record[m].tag) + strlen(record[m].info) > 255){
				printf("longer than 255  characters\n");
				errorCheck.type = INV_RECORD;
				errorCheck.line = record[m].line;
				return errorCheck;
			}
			if ((record[m+1].level - record[m].level) > 1){
				printf("line diff is greate than one\n");
				errorCheck.type = INV_RECORD;
				errorCheck.line = record[m + 1].line;
				return errorCheck;
			}
		}//validating recrod for loop ends
        object->header = headParser(record, k, &pointers, &receiver);
      }
      else if (strcmp(record[0].tag, "SUBM") == 0){
        object->submitter = subParser(record, k, &pointers, &receiver);
        object->header->submitter = object->submitter;
        subFlag = 1;
        //printf("objects submiter Name : %s\n", object->submitter->submitterName);
        //printf("passed submitter\n");
      }
      else if (strcmp(record[0].tag, "INDI") == 0){
        //printf("before parsing indi\n");
        dummyPerson = parseIndividual(record, k, &pointers, &receiver);
        dummyPerson->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
        insertBack(&people, dummyPerson);
        //printf("indi passed\n");
      }
      else if (strcmp(record[0].tag, "FAM") == 0){
        dummyFamily = parseFamily(record, k, people);
        insertBack(&family, dummyFamily);
      }
    }else{
		//printf("start is not 0\n");
		//printf("info level: %d tag: %s level: %s\n", info[i].level, info[i].tag, info[i].info);
	  if (strcmp(info[i].tag, "HEAD") == 0){
		if (info[i].level > 0){
			//printf("header level not zero\n");
			errorCheck.type = INV_HEADER;
			errorCheck.line = -1;
			return errorCheck;
		}
	  }
    }
  }
  if (subFlag == 0){
	  errorCheck.type = INV_GEDCOM;
	  printf("Fuck inbalid\n");
	  errorCheck.line = -1;
	  return errorCheck;
  }
  if (strcmp(info[length-1].tag, "TRLR") != 0){
	  errorCheck.type = INV_GEDCOM;
	  errorCheck.line = info[length-1].line;
	  return errorCheck;
  }

  object->individuals = people;
  object->families = family;

  *obj = object;

  for (int i = 0; i < 1000; i++){ // freeing the allocated memory after done parsing the file
    free(read[i]);//freeing the allocated strings
  }
  free(read);
  free(info);
  free(record);
  //printf("before returning from create GEDCOM of <<%s>>\n", fileName);
  
  errorCheck.type = OK;//setting the right type 
  errorCheck.line = -1;
  
  return errorCheck;
}


/** Function to create a string representation of a GEDCOMobject.
 *@pre GEDCOMobject object exists, is not null, and is valid
 *@post GEDCOMobject has not been modified in any way, and a string representing the GEDCOM contents has been created
 *@return a string contaning a humanly readable representation of a GEDCOMobject
 *@param obj - a pointer to a GEDCOMobject struct
 **/
char* printGEDCOM(const GEDCOMobject* obj){
	/*printf("inside print GEDFCOm\n");
	char * string = malloc(sizeof(char)*500);
	
	if (obj == NULL){
		return "\0";
	}*/
	//sprintf(string, "SOUR: %s GEDOM VERSION: %lf Submitter: %s\n", obj->header->source, obj->header->gedcVersion, obj->submitter->submitterName);
	/*if (obj->individuals.head != NULL){
		char indiString[40];
		strncpy(indiString, "Number of Individual %d\n", obj->individuals.length);
		strcat(string, indiString);
		//strcat(string, toString(obj->individuals));
	}
	if (obj->families.head != NULL){
		char famString[50];
		strncpy(                                                                                                                                                                      famString, "Number of fmailies %d\n", obj->families.length);
		strcat(string, famString);
		//strcat(string, toString(obj->families));
	}*/
	char* tmpStr;
	int len = 0;
	
	if (obj == NULL){
	    return "\0";
	}
		
	len = strlen(obj->header->source) + strlen(obj->submitter->submitterName) + 200;
	tmpStr = (char*)malloc((sizeof(char)*len));
	
	sprintf(tmpStr, "Source: %s\nGEDCOM version: %.1lf\nSubmitter: %s\n", obj->header->source, obj->header->gedcVersion, obj->submitter->submitterName);
	len = len + (obj->individuals.length)*60;
	tmpStr = realloc(tmpStr, (sizeof(char)*len));
	Node* tmpNode = obj->individuals.head;
	Individual* tmpIndi;
	strcat(tmpStr, "Individuals:\n\n");
	for (int i=0; i<(obj->individuals.length); i++) {
		tmpIndi = tmpNode->data;
        strcat(tmpStr, (tmpIndi->givenName));
        strcat(tmpStr, " ");
        strcat(tmpStr, (tmpIndi->surname));
        strcat(tmpStr, "\n\n");
        tmpNode = tmpNode->next;
    }
    len = len + (obj->families.length)*200;
    tmpStr = realloc(tmpStr, (sizeof(char)*len));
	tmpNode = obj->families.head;
	Family* tmpFam;
	strcat(tmpStr, "Families:\n\n");
	for (int i=0; i<(obj->families.length); i++) {
		tmpFam = tmpNode->data;
		strcat(tmpStr, "Wife: ");
		if (tmpFam->wife!=NULL) {
            strcat(tmpStr, (tmpFam->wife->givenName));
            strcat(tmpStr, " ");
            strcat(tmpStr, (tmpFam->wife->surname));
	    }
        strcat(tmpStr, "\n");
        strcat(tmpStr, "Husband: ");
        if (tmpFam->husband!=NULL) {
            strcat(tmpStr, (tmpFam->husband->givenName));
            strcat(tmpStr, " ");
            strcat(tmpStr, (tmpFam->husband->surname));
	    }
        strcat(tmpStr, "\n");
        char kids[10];
        sprintf(kids, "Number of kids: %d", tmpFam->children.length);
        strcat(tmpStr, kids);
        strcat(tmpStr, "\n\n");
        tmpNode = tmpNode->next;
    }
    return tmpStr;	
	
 //return string;
}

/** Function to delete all GEDCOM object content and free all the memory.
 *@pre GEDCOM object exists, is not null, and has not been freed
 *@post GEDCOM object had been freed
 *@return none
 *@param obj - a pointer to a GEDCOMobject struct
 **/
void deleteGEDCOM(GEDCOMobject* obj){
  if (obj == NULL){
    return ;
  }
  clearList(&obj->header->otherFields);
  clearList(&obj->individuals);
  clearList(&obj->families);
  clearList(&obj->submitter->otherFields);
  free(obj->submitter);
  free(obj);
}


/** Function to "convert" the GEDCOMerror into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code
 *@param err - an error struct
 **/
char* printError(GEDCOMerror err){
	char * string = malloc(sizeof(char) * 20);
	strcpy(string, "");
	if (err.type == INV_FILE){
		strcat(string, "Inavlid File\n");
	}
	else if (err.type == INV_HEADER){
		strcat(string, "Invalid Header\n");
	}
	else if (err.type == INV_GEDCOM){
		strcat(string, "Invalid GEDCOM\n");
	}
	else if (err.type == INV_RECORD){
		strcat(string, "Invalid Record\n");
	}
	else if (err.type == OTHER_ERROR){
		strcat(string, "Other Error\n");
	}
	else if (err.type == OK){
		strcat(string, "OK");
	}
	
  return string;
}

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
Individual* findPerson(const GEDCOMobject* failyRecord, bool (*compare)(const void* first, const void* second), const void* person){
	Individual * indi = NULL;
	
	if (failyRecord == NULL){
		return NULL;
	}
	indi = findElement(failyRecord->individuals, compare, person);
	
  return indi;
}


/** Function to return a list of all descendants of an individual in a GEDCOM
 *@pre GEDCOM object exists, is not null, and is valid
 *@post GEDCOM object has not been modified in any way, and a list of descendants has been created
 *@return a list of descendants.  The list may be empty.  All list members must be of type Individual, and can appear in any order.
 *All list members must be COPIES of the Individual records in the GEDCOM file.  If the returned list is freed, the original GEDCOM
 *must remain unaffected.
 *@param failyRecord - a pointer to a GEDCOMobject struct
 *@param person - the Individual record whose descendants we want
 **/
List getDescendants(const GEDCOMobject* failyRecord, const Individual* person){
  List hey = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
  
  if (failyRecord == NULL || person == NULL){
	  return hey;
  }
  return hey;
}


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
  //int length;
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
  
  char name1[50];
  
  strcat(name1, compare1->givenName);
  strcat(name1, ", ");
  strcat(name1, compare1->surname);

  return strcmp(compare1->surname, compare2->surname);
}

char* printIndividual(void* toBePrinted){
  char* string;
  //char * events;
  //char * other;
  Individual* print;
  if (toBePrinted == NULL){
    return NULL;
  }
  printf("in here\n");
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
  string = (char*)malloc(100000 * sizeof(char));
  //printf("in print individual\n");
  sprintf(string, "FirstName: %s, LastName: %s", print->givenName, print->surname);
  //events = toString(print->events);
  //other = toString(print->otherFields);
  //puts(events);
  //puts(other);
  if (print->events.head != NULL){
    strcat(string, toString(print->events));
  }
  //printf("before indi famlist\n");
  // if (print->families.head != NULL){
  //   strcat(string, toString(print->families));
  // }
  //printf("after indi famlist\n");
  if (print->otherFields.head != NULL){
    strcat(string, toString(print->otherFields));
  }
  //strcat(string, toString(print->events));
  //strcat(string, toString(print->families));
  //strcat(string, toString(print->otherFields));
  return string;
}

void deleteFamily(void* toBeDeleted){
  Family * Delete;

  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Family*)toBeDeleted;
  free(Delete);
}

int compareFamilies(const void* first,const void* second){
  Family * compare1;
  Family * compare2;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Family*)first;
  compare2 = (Family*)second;

  int count1 = 0;
  int count2 = 0;

  if (compare1->wife != NULL){
    count1++;
  }
  if (compare1->husband != NULL){
    count1++;
  }
  if (compare1->children.head != NULL){
    count1 = getLength(compare1->children) + count1;
  }

  if (compare2->wife != NULL){
    count2++;
  }
  if (compare2->husband != NULL){
    count2++;
  }
  if (compare2->children.head != NULL){
    count2 = getLength(compare1->children) + count2;
  }

  return count1 - count2;
}

char* printFamily(void* toBePrinted){
  char * string;
  Family * print;

  if (toBePrinted == NULL){
    return NULL;
  }
  
  string = calloc(10000000, sizeof(char));
  print = (Family*)toBePrinted;
  
  strcat(string, "WIFE:\n");
  strcat(string, printIndividual(print->wife));
  
  strcat(string, "HUSBAND:\n");
  strcat(string, printIndividual(print->husband));
  
  char temp[40];
  sprintf(temp, "Number of Children %d\n", print->children.length);
  strcat(string, temp);
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
  Field * compare1 = (Field*)first;
  Field * compare2 = (Field*)second;

  if (first == NULL || second == NULL){
    return 0;
  }

  compare1 = (Field*)first;
  compare2 = (Field*)second;


  char * combine1 = malloc(sizeof(char) * 500);
  char * combine2 = malloc(sizeof(char) * 500);

  strcpy(combine1, compare1->tag);
  strcat(combine1, " ");
  strcpy(combine1, compare1->value);

  strcpy(combine2, compare2->tag);
  strcat(combine2, " ");
  strcpy(combine2, compare2->value);

  return strcmp(combine1, combine2);
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
