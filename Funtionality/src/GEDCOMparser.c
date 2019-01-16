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
  
  List reference = initializeList(printReference, deleteReferenceList, dummyCompare);

  Individual * dummyPerson /*= calloc(1, sizeof(Individual))*/;

  Family * dummyFamily /*= calloc(1, sizeof(Family))*/;

  GEDCOMobject * object = malloc(sizeof(GEDCOMobject));
  
  if (validateFile(fileName) == INV_FILE){
    errorCheck = setType(INV_FILE, -1);
    obj = NULL;
    return errorCheck;
  }
  
  //*obj = malloc(sizeof(GEDCOMobject));
  //printf("\nparsing this file |%s|\n", fileName);
  read = fileReader(fileName);
  //printf("finding long ass line || %s\n", read[2]);
  length = fileLength(read);
  info = calloc(1000000, sizeof(Info));
  record = calloc(1000000, sizeof(Info));

  /**
  ** this function tockenizes the line of GEDCOM line and put it into information struct
  **/
  //printf("\n after validateing parsing this file |%s|\n", fileName);
  for (int i = 0; i < length; i++){
    info[i] = tockenInfo(read[i]);
    info[i].line = i + 1;
  }
  //printf("fitst one %d\n", info[length-1].level);
  //printf("second one %s\n", info[length-1].tag);
  //printf("third one %s\n", info[length-1].info);
  //printf("last line of file %d || %s || %s || %d\n", info[length-1].level, info[length-1].tag, info[length-1].info, length-1);
  if (strcmp(info[length-1].tag, "TRLR") != 0){
	  //printf("trlr is nor there\n");
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
				//printf("longer than 255  characters\n");
				errorCheck.type = INV_RECORD;
				errorCheck.line = record[m].line;
				return errorCheck;
			}
			if ((record[m+1].level - record[m].level) > 1){
				//printf("line diff is greate than one\n");
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
        dummyPerson = parseIndividual(record, k, &pointers, &receiver, &reference);
        dummyPerson->families = initializeList(&printFamily, &dummyDelete, &compareFamilies);
        insertBack(&people, dummyPerson);
      }
      else if (strcmp(record[0].tag, "FAM") == 0){
        dummyFamily = parseFamily(record, k, people, &reference);
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
	  //printf("Fuck inbalid\n");
	  errorCheck.line = -1;
	  return errorCheck;
  }
  if (strcmp(info[length-1].tag, "TRLR") != 0){
	  errorCheck.type = INV_GEDCOM;
	  errorCheck.line = info[length-1].line;
	  return errorCheck;
  }

  /*Node * node = people.head;//
  Individual * one;
  Node * field;
  Field * second;
  while(node != NULL){
    one = (Individual*)node->data;
    field = one->otherFields.head;
    while(field != NULL){
      second = (Field*)field->data;
      if (second->value[0] == '@'){
        //printf("xref found\n");
        deleteDataFromList(&one->otherFields, second);
        //puts(toString(one->otherFields));
      }
      //puts(toString(one->otherFields));
      field = field->next;
    }
    node = node->next;
  }*/
	
  clearList(&reference);
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
	char* tmpStr;
	int len = 0;
	
	if (obj == NULL){
	    return "\0";
	}
		
	len = strlen(obj->header->source) + strlen(obj->submitter->submitterName) + 999;
	tmpStr = (char*)malloc((sizeof(char)*len));
	
	sprintf(tmpStr, "Source: %s\nGEDCOM version: %.1lf\nSubmitter: %s\n", obj->header->source, obj->header->gedcVersion, obj->submitter->submitterName);

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
  free(obj->header);
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
List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){
  List descendents = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
  //bool (*compare)(const void* first, const void* second) = customCompare;
  
  if (familyRecord == NULL || person == NULL){
	  return descendents;
  }

  Individual * found = findPerson(familyRecord, comparePerson, person);
  descendents = collectDescendents(found, &descendents);
  //puts(toString(descendents));
  return descendents;
}

/** Function for validating an existing GEDCOM 
object 
 *@pre GEDCOM
 object exists and is not null 
 *@post GEDCOM object
 has not been modified in any way 
 *@return the error code indicating success or the error encountered when 
validating the GEDCOM
 *@param obj - a pointer to a GEDCOMobject
 struct 
 **/
ErrorCode validateGEDCOM(const GEDCOMobject* obj){
  ErrorCode error;
  
  if (obj == NULL){
	  return INV_GEDCOM;
  }
  if (obj->header == NULL || obj->submitter == NULL){
    error = INV_GEDCOM;
    return error;
  }
  if (obj->header->source != NULL){
	  if (strlen(obj->header->source) == 0){
		error = INV_HEADER;
		return error;
	  }
  }
  if (obj->header->source == NULL || obj->header->gedcVersion == 0 || obj->header->submitter == NULL){
	  error = INV_HEADER;
	  return error;
  }
  /*else if (obj->header->gedcVersion == 0){
    error = INV_HEADER;
    return error;
  }
  else if (obj->header->submitter == NULL){
    error = INV_HEADER;
    return error;
  }*/
  else if (obj->header->encoding != ASCII && obj->header->encoding !=UTF8 && obj->header->encoding != ANSEL && obj->header->encoding != UNICODE){
    error = INV_HEADER;
    return error;
  }
  else if (obj->header->otherFields.length > 0){
	  if(validateField(obj->header->otherFields) == INV_RECORD){
		  return INV_RECORD;
	  }
  }
  else if (obj->submitter == NULL){
	  error = INV_RECORD;
	  return error;
  }
  else if (obj->submitter != NULL){
	  if (strlen(obj->submitter->submitterName) == 0){
		error = INV_RECORD;
		return error;
	}
  }
  /*else if (strlen(obj->submitter->submitterName) == 0){
    error = INV_RECORD;
    return error;
  }*/
  else if (obj->submitter->otherFields.length > 0){
	  if(validateField(obj->submitter->otherFields) == INV_RECORD){
		  return INV_RECORD;
	  }
  }
  
  Node * indi_node = obj->individuals.head;
  for (int i = 0; i < obj->individuals.length; i++){
	  
		Individual * test = (Individual*)indi_node->data;
		if (test->givenName != NULL){
			if (strlen(test->givenName) > 200){
				return INV_RECORD;
			}
		}
		if (test->surname != NULL){
			if (strlen(test->surname) > 200){
				return INV_RECORD;
			}
		}
	     Node * fam = test->families.head;
	     if (test->families.head != NULL){//if the family field is not empty 
			//Node * fam = test->families.head;
			for(int i = 0; i < test->families.length; i++){
				//printf("family head is not null\n");
				Family * check = (Family*)fam->data;
				if (check == NULL){//when the pointer of that Family is NULL
					//printf("check is null\n");
					error = INV_RECORD;
					return error;
				}
				else if (check != NULL){//when the node is not empty
					Node * fam_other = check->otherFields.head;
					if(fam_other != NULL){
						if (validateField(check->otherFields) == INV_RECORD){
							error = INV_RECORD;
							return error;
							
						}
						//fam_other = fam_other->next;
					}
					Node * children = check->children.head;
					while(children != NULL){
						Individual * child = (Individual*)children->data;
						if (child == NULL){
							return INV_RECORD;
						}
						children = children->next;
					}
					if (validateEvent(check->events) == INV_RECORD){
						return INV_RECORD;
					}
				}
				//fam = fam->next;
			}
			fam = fam->next;
			//printf("um.....\n");
		}//end if family field is not empty
		//Node * events = test->evetns.head;
		if (validateEvent(test->events) == INV_RECORD){
			error = INV_RECORD;
			return error;
		}
		if (test->otherFields.length > 0){
			if (validateField(test->otherFields) == INV_RECORD){
				return INV_RECORD;
			}
		}
	  indi_node = indi_node->next;
	}
	error = OK;
	
  return error;
}

GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj){
  GEDCOMerror error;

  //List references = initializeList(printPointers, deleteReference, dummyCompare);
  
  if (fileName == NULL){
	  error.type = WRITE_ERROR;
	  error.line = -1;
	  return error;
  }
  
  //int size = 1;
  if (obj == NULL){
    error.type = WRITE_ERROR;
    error.line = -1;
    return error;
  }

  FILE * toWrite = fopen(fileName , "w");
  char encode[10] = "";

  if (toWrite == NULL)
  {
    error.type = WRITE_ERROR;
    error.line = -1;
    return error;
  }
  int level = 0;
  if (obj->header != NULL){//when the header pointer is not null
    fprintf(toWrite, "%d %s\n", level++, "HEAD");
    fprintf(toWrite, "%d %s %s\n", level, "SOUR", obj->header->source);
    fprintf(toWrite, "%d %s\n", level++, "GEDC");
    fprintf(toWrite, "%d %s %.2lf\n", level, "VERS", obj->header->gedcVersion);
    fprintf(toWrite, "%d %s %s\n", level--, "FORM", "LINEAGE-LINKED");
    if (obj->header->encoding == ANSEL){
      strcpy(encode, "ANSEL");
    }else if (obj->header->encoding == UTF8){
      strcpy(encode, "UTF-8");
    }else if(obj->header->encoding == UNICODE){
      strcpy(encode, "UNICODE");
    }else if (obj->header->encoding == ASCII){
      strcpy(encode, "ASCII");
    }
    fprintf(toWrite, "%d %s %s\n", level, "CHAR", encode);
    fprintf(toWrite, "%d %s %s\n", level, "SUBM", "@SUB1@");
  }

  if (obj->submitter != NULL){//when the submitter pointer is not null
    level = 0;
    fprintf(toWrite, "%d %s %s\n", level++, "@SUB1@", "SUBM");
    fprintf(toWrite, "%d %s %s\n", level, "NAME", obj->submitter->submitterName);
    if (obj->submitter->address != NULL){
		if (strlen(obj->submitter->address) != 0){
		fprintf(toWrite, "%d %s %s\n", level, "ADDR", obj->submitter->address);
		}
	}
  }
    List IndividualRefList = initializeList(NULL, deleteIndiReference, dummyCompare);
    List FamilyRefList = initializeList(NULL, deleteFamReference, dummyCompare);
    Node * person = obj->individuals.head;
    int indi = 1;
    //IndividualRef * personRef = calloc(1, sizeof(IndividualRef));
    while(person != NULL){
      Individual * current = (Individual*)person->data;
      char temp[10] = "";
      IndividualRef * personRef = calloc(1, sizeof(IndividualRef));
      sprintf(temp, "@I%d@", indi++);
      strcpy(personRef->indiRef,temp);
      personRef->point = current;
      personRef->famsRef = initializeList(NULL, dummyDelete, NULL);
      personRef->famcRef = initializeList(NULL, dummyDelete, NULL);
      insertBack(&IndividualRefList, personRef);
      person = person->next;
    }
    int fam = 1;
    Node * Families = obj->families.head;
    //FamilyRef * famRef = calloc(1,sizeof(FamilyRef));

    while(Families != NULL){
      Family * current = (Family*)Families->data;
      char temp[10];
      FamilyRef * famRef = calloc(1,sizeof(FamilyRef));
      sprintf(temp, "@F%d@", fam++);
      strcpy(famRef->famRef, temp);
      famRef->point = current;
      famRef->children = initializeList(NULL, dummyDelete, NULL);
      Node * iterate = IndividualRefList.head;
      //IndividualRef * ptr = (IndividualRef*)iterate->data;
      //ptr->famsRef = initializeList(NULL, dummyDelete, NULL);
      //ptr->famcRef = initializeList(NULL, dummyDelete, NULL);
      while(iterate != NULL){
         IndividualRef * ptr = (IndividualRef*)iterate->data;
         //ptr->famsRef = initializeList(NULL, dummyDelete, NULL);
         //ptr->famcRef = initializeList(NULL, dummyDelete, NULL);

        if (current->wife == ptr->point){//if the pointer of individual matches the wife pointer
          //printf("it is wife\n");
          insertBack(&ptr->famsRef, famRef->famRef);
          //printf("checking if inserted |%s| |%s|\n", (char*)ptr->famsRef.head->data, ptr->point->givenName);
          strcpy(famRef->wifeRef, ptr->indiRef);
        }

        if (current->husband == ptr->point){// if the pointer of individual mathces the husband pointer
          //printf("it is husband\n");
          insertBack(&ptr->famsRef, famRef->famRef);
          //printf("checking if inserted |%s|\n", (char*)ptr->famsRef.head->data);
          strcpy(famRef->husbandRef, ptr->indiRef);
        }

        Node * child = current->children.head;
       // famRef->children = initializeList(NULL, dummyDelete, NULL);
        while(child != NULL){//iterating through the list of children of the family
          Individual * this = (Individual*)child->data;
          if (this == ptr->point){//if the pointer of child matches the pointer of individual
            //printf("it is child\n");
            insertBack(&famRef->children, ptr->indiRef);
            insertBack(&ptr->famcRef, famRef->famRef);
          }
          child = child->next;
        }
        /****************************************************************/
        //printf("beforecheck\n");
        //IndividualRef * toCheck = (IndividualRef*)iterate->data;
        //printf("before Node\n");
        iterate = iterate->next;
      }
      insertBack(&FamilyRefList, famRef);
      Families = Families->next;
    }
    
    Node * iterateIndiRef = IndividualRefList.head;
    //level = 0;
    while(iterateIndiRef != NULL){
      level = 0;
      IndividualRef * reference = (IndividualRef*)iterateIndiRef->data;
      fprintf(toWrite, "%d %s %s\n", level++, reference->indiRef, "INDI");
      Individual * IndiToPrint = reference->point;
      if (IndiToPrint->givenName == NULL){
        IndiToPrint->givenName = "";
      }else if (IndiToPrint->surname == NULL){
        IndiToPrint->surname = "";
      }
      fprintf(toWrite, "%d %s %s /%s/\n", level, "NAME", IndiToPrint->givenName, IndiToPrint->surname);
      Node * other = IndiToPrint->otherFields.head;
      while(other != NULL){
        Field * data = (Field*)other->data;
        //printf("tag: %s value: %s\n", data->tag, data->value);
        if (strcmp(data->tag, "GIVN") == 0){
          level = 2;
          //printf("givenName\n");
          fprintf(toWrite, "%d %s %s\n", level, "GIVN", data->value);
        }
        else if (strcmp(data->tag, "SURN") == 0){
          level = 2;
          fprintf(toWrite, "%d %s %s\n", level, "SURN", data->value);
        }else{
			level = 1;
		  fprintf(toWrite, "%d %s %s\n", level, data->tag, data->value);
		}
        other = other->next;
      }
      Node * events = IndiToPrint->events.head;
      //printf("before indi events\n");
      while(events != NULL){
        level = 1;
        //printf("indi events exits\n");
        Event * data = (Event*)events->data;
        fprintf(toWrite, "%d %s\n", level++, data->type);
        if (data->date != 0){
          fprintf(toWrite, "%d %s %s\n", level, "DATE", data->date);
        }
        if (data->place != 0){
          fprintf(toWrite, "%d %s %s\n", level, "PLAC", data->place);
        }
        //fprintf(toWrite, "%d %s %s\n", level, data->)
        events = events->next;
      }
      Node * famsNode = reference->famsRef.head;
      //printf("for this individual |%s|\n", IndiToPrint->givenName);
      //printf("length of spouse ref list %d\n", reference->famsRef.length);
      //printf("length of children ref list %d\n", reference->famcRef.length);
      while(famsNode != NULL){
        level = 1;
        fprintf(toWrite, "%d %s %s\n", level, "FAMS", (char*)famsNode->data);
        //printf("spouse node exist |%s|\n", (char*)famsNode->data);
        famsNode = famsNode->next;
      }
      Node * famcNode = reference->famcRef.head;
      while(famcNode != NULL){
        level = 1;
        fprintf(toWrite, "%d %s %s\n", level, "FAMC", (char*)famcNode->data);
        //printf("child node exist |%s|\n", (char*)famcNode->data);
        famcNode = famcNode->next;
      }
      iterateIndiRef = iterateIndiRef->next;
    }

    Node * iterateFamRef = FamilyRefList.head;
    while(iterateFamRef != NULL){
      level = 0;
      FamilyRef * reference = (FamilyRef*)iterateFamRef->data;
      fprintf(toWrite, "%d %s %s\n", level++, reference->famRef, "FAM");
      fprintf(toWrite, "%d %s %s\n", level, "WIFE", reference->wifeRef);
      fprintf(toWrite, "%d %s %s\n", level, "HUSB", reference->husbandRef);
      Family * fam = reference->point;
      Node * events = fam->events.head;
      //printf("before fam events length : <%d>\n", fam->events.length);
      while(events != NULL){
        level = 1;
        //printf("fam events exits\n");
        Event * data = (Event*)events->data;
        fprintf(toWrite, "%d %s\n", level++, data->type);
        //printf("after writing type of eventts\n");
        if (data->date != NULL){
          fprintf(toWrite, "%d %s %s\n", level, "DATE", data->date);
          //printf("date is written\n");
        }
        if (data->place != NULL){
          //printf("in here\n");
          fprintf(toWrite, "%d %s %s\n", level, "PLAC", data->place);
          //printf("place is written\n");
        }
        events = events->next;
      }
      Node * childNode = reference->children.head;
      level = 1;
      while(childNode != NULL){
        //printf("writing children to file\n");
        fprintf(toWrite, "%d %s %s\n", level, "CHIL", (char*)childNode->data);
        childNode = childNode->next;
      }
      //printf("before goin next\n");
      iterateFamRef = iterateFamRef->next;
      //printf("after going next\n");
    }


  clearList(&FamilyRefList);
  clearList(&IndividualRefList);
  level = 0;
  fprintf(toWrite, "%d %s", level, "TRLR");
  fclose(toWrite);
  
  error.type = OK;
  error.line = -1;

  return error;
}
/** Function to return a list of up to N generations of
 descendants of an individual in a 
GEDCOM 
 *@pre GEDCOM object exists, is not null, and is valid 
 *@post GEDCOM object has not been modified in any way, and a list of descendants has been 
created 
 *@return a list of descendants.  The list may be empty.  All list members must be of type 
List.  
 *@param familyRecord - a pointer to a GEDCOMobject struct 
 *@param person - the Individual record whose descendants we want
 *@param maxGen - maximum number of generations to examine (must be >= 1)
 **/
List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen){
  List All = initializeList(&printStruct, &deleteStruct, &dummyCompare);
  List final = initializeList(&printGeneration, &deleteGeneration, dummyCompare);
  List subMaster = initializeList(NULL, &deleteGenStruct, dummyCompare);

  if (familyRecord == NULL || person == NULL){
	  return final;
  }
  int var = 0;
  int max = 0;
  Individual * found = findPerson(familyRecord, comparePerson, person);
  All = DescendentWrapper(found, &All, var);
  //Generation * gen;
  //puts(toString(All));
  //Node * node = All.head;
    Node * node = All.head;
  while(node != NULL){
    EachGen * data = (EachGen*)node->data;
    if (max < data->level){
      max = data->level;
    }
    node = node->next;
  }
  //printf("max = %d\n", max);
  //if (maxGen != 0){
    if (maxGen > max || maxGen == 0){
      maxGen = max;
    }
    
    //Generation * gen = calloc(maxGen,sizeof(Generation));
    int genCount = 0;
    while(genCount != maxGen){
		Generation * gen = calloc(1, sizeof(Generation));
		gen->level = genCount + 1;
		gen->generation = calloc(1, sizeof(List));
		*gen->generation = initializeList(printIndividual, deleteIndividual, compareIndividuals);
		insertBack(&subMaster, gen);
		genCount++;
	}
	//printf("sizeof the submaster %d\n", subMaster.length);

    Node * node2 = All.head;
    while(node2 != NULL){
      EachGen * data = (EachGen*)node2->data;
      Node * subNode = subMaster.head;
      while(subNode != NULL){
		  Generation * temp = (Generation*)subNode->data;
		  if (data->level == temp->level){//when the levels match
			  //printf("found the same level\n");
			  //Individual * copy = copyIndividual(data->indi);
		      insertSorted(temp->generation, data->indi);
		      //puts(toString(*temp->generation));
		  }
		  subNode = subNode->next;
	  }
      node2 = node2->next;
    }
    Node * putting = subMaster.head;
	while(putting != NULL){
		Generation * temp = (Generation*)putting->data;
		insertBack(&final, temp->generation);
		putting = putting->next;
	}
  clearList(&All);
  clearList(&subMaster);
  return final;
}

List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen){
  List All = initializeList(&printStruct, &deleteStruct, &dummyCompare);
  List final = initializeList(&printGeneration, &deleteGeneration, dummyCompare);
  List subMaster = initializeList(NULL, &deleteGenStruct, dummyCompare);

  if (familyRecord == NULL || person == NULL){
    return final;
  }

  int var = 0;
  int max = 0;
  Individual * found = findPerson(familyRecord, comparePerson, person);
  All = AncestorWrapper(found, &All, var);
  //printf("length of sorted list %d\n", All.length);
  //puts(toString(All));
  //Node * node = All.head;
  Node * node = All.head;
  while(node != NULL){
    EachGen * data = (EachGen*)node->data;
    if (max < data->level){
      max = data->level;
    }
    node = node->next;
  }
  //printf("max = %d\n", max);
  //if (maxGen != 0){
    if (maxGen > max || maxGen == 0 || maxGen < 0){
      maxGen = max;
    }
    
    //Generation * gen = calloc(maxGen,sizeof(Generation));
    int genCount = 0;
    while(genCount != maxGen){
		Generation * gen = calloc(1, sizeof(Generation));
		gen->level = genCount + 1;
		gen->generation = calloc(1, sizeof(List));
		*gen->generation = initializeList(printIndividual, deleteIndividual, compareIndividuals);
		insertBack(&subMaster, gen);
		genCount++;
	}
	//printf("sizeof the submaster %d\n", subMaster.length);
	int flag = 0;
    Node * node2 = All.head;
    while(node2 != NULL){
      EachGen * data = (EachGen*)node2->data;
      Node * subNode = subMaster.head;
      while(subNode != NULL){
		  Generation * temp = (Generation*)subNode->data;
		  if (data->level == temp->level){//when the levels match
			  //printf("found the same level\n");
			  //Individual * copy = copyIndividual(data->indi);
			  Node * iter = temp->generation->head;
			  while(iter != NULL){
				  Individual * per = (Individual*)iter->data;
				  if (strcmp(per->givenName, data->indi->givenName) == 0 && strcmp(per->surname, data->indi->surname) == 0){
					flag = 1;
				  }
				  iter = iter->next;
			  }
			  if (flag != 1){
				  insertSorted(temp->generation, data->indi);
			  }
		      //insertSorted(temp->generation, data->indi);
		      //puts(toString(*temp->generation));
		  }
		  subNode = subNode->next;
		  flag = 0;
	  }
      node2 = node2->next;
    }
    Node * putting = subMaster.head;
	while(putting != NULL){
		Generation * temp = (Generation*)putting->data;
		insertBack(&final, temp->generation);
		putting = putting->next;
	}
  clearList(&All);
  clearList(&subMaster);
  return final;
}

char* indToJSON(const Individual* ind){
  char * string = "";
  //int len = 0;
  //printf("called address %p \n", (void*)ind);
  string = calloc(10000, sizeof(char));
  if (ind == NULL){
    //string = calloc(1, sizeof(char));
    //sprintf(string, "");
    strcpy(string, "");
    return string;
  }
  // printf("called address %p \n", ind);
  else if (ind->givenName == NULL){
    //len = strlen(ind->surname);
    //string = calloc(len+99, sizeof(char));
    sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", "", ind->surname);
  }
  else if (ind->surname == NULL){
    //len = strlen(ind->givenName);
    //string = calloc(len+99, sizeof(char));
    sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", ind->givenName, "");
  }
  else{
    //if (ind->givenName != NULL || ind->surname != NULL){
      //len = (strlen(ind->givenName)) + (strlen(ind->surname));
      //string = calloc(len+99, sizeof(char));
      sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", ind->givenName, ind->surname);
    //}
  }
  return string;
}

Individual* JSONtoInd(const char* str){
  Individual * me = calloc(1, sizeof(Individual));
  int i = 0;
  char * temp = (char*)str;
  char * token = strtok(temp, "{}\",:");
  char array[4][300];
  int counter = 0;
  while(token != NULL){
    strcpy(array[i], token);
    token = strtok(NULL, "{},\":");
    //printf("array %s\n", array[i]);
    i++;
  }
  for (int j = 1; j < i; j++){
    if (strcmp(array[j-1], "givenName") == 0){
      me->givenName = calloc(strlen(array[j]) + 90,sizeof(char));
      if (strcmp(array[j], "surname") == 0){
        strcpy(me->givenName, "");
      }
      else{
        strcpy(me->givenName, array[j]);
      }
      counter++;
    }
    
    else if (strstr(array[j-1], "surname") != NULL){
      me->surname = calloc(strlen(array[j]) + 90,sizeof(char));
      if (array[j] == NULL){
        strcpy(me->surname, "");
      }else{
        strcpy(me->surname, array[j]);
      }
      counter++;
    }
    
    else if (j == (i - 1)){
      me->surname = calloc(1,sizeof(char));
      strcpy(me->surname, "");
      counter++;
    }
    me->events = initializeList(printEvent, deleteEvent, compareEvents);
    me->families = initializeList(printFamily, deleteFamily, compareFamilies);
    me->otherFields = initializeList(printField, deleteField, compareFields);
    //printf("indi <%s> <%s>\n", me->givenName, me->surname);
  }
  //printf("counter %d\n", counter);
  if (counter < 2){
	  return NULL;
  }
  //printf("indi <%s> <%s>\n", me->givenName, me->surname);
  return me;
}

GEDCOMobject* JSONtoGEDCOM(const char* str){
 GEDCOMobject * object;
 Header * head = calloc(1, sizeof(Header));
 head->otherFields.head = NULL;
 Submitter * sub = calloc(1000, sizeof(Submitter));
 sub->otherFields.head = NULL;
 CharSet charType;
 //printf("printing str <%s>\n", str);
 if (str == NULL){
   return NULL;
 }
 if (strlen(str) == 0){
	 return NULL;
 }
 char array[20][300];
 char * token = strtok((char*)str, ":\"{},");
 if (strcmp(token, "source") != 0){
	 return NULL;
 }
 int i = 0;
 while(token != NULL){
   strcpy(array[i], token);
   //printf("array <%s>\n", array[i]);
   i++;
   token = strtok(NULL, ":\"{},");
 }
 for (int j = 0 ; j < i; j++){
   //printf("in the while\n");
   if (strcmp(array[j], "source") == 0){
     //printf("source\n");
     j++;
     printf("source: %s\n", array[j]);
     if (strcmp(array[j], "gedcVersion") == 0 || strcmp(array[j], "encoding") == 0 || strcmp(array[j], "subName") == 0 || strcmp(array[j], "subAddress") == 0){
       return NULL;
     }
     
     strcpy(head->source, array[j]);
   }
   else if (strcmp(array[j], "gedcVersion") == 0){
     j++;
     //printf("version\n");
     float ver = 0.0;
     printf("gedcversion: %s\n", array[j]);
     if (strcmp(array[j], "encoding") == 0 || strcmp(array[j], "subName") == 0 || strcmp(array[j], "subAddress") == 0){
       return NULL;
     }
     ver = atof(array[j]);
     head->gedcVersion = ver;
   }
   else if (strcmp(array[j], "encoding") == 0){
     j++;
     
     printf("encoding: %s\n", array[j]);
     
     if (strcmp(array[j], "subName") == 0 || strcmp(array[j], "subAddress") == 0){
       return NULL;
     }
     if (strcmp(array[j], "ASCII") == 0){
       //printf("this is ASCII\n");
       charType = ASCII;
     }else if (strcmp(array[j], "UTF8") == 0 || strcmp(array[j], "UTF-8") == 0){
       charType = UTF8;
     }else if (strcmp(array[j], "UNICODE") == 0){
       charType = UNICODE;
     }else if (strcmp(array[j], "ANSEL") == 0){
       charType = ANSEL;
     }
     head->encoding = charType;
   }
   else if (strcmp(array[j], "subName") == 0){
     j++;
     printf("subname: %s\n", array[j]);
     if (strcmp(array[j], "subAddress") == 0){
       return NULL;
     }
     strcpy(sub->submitterName, array[j]);
   }
   else if (strcmp(array[j], "subAddress") == 0){
     printf("subaddress: %s\n", array[j]);
     j++;
     if (j == i){
       strcpy(sub->address, "");
     }else{
		 strcpy(sub->address, array[j]);
	 }
    //strcpy(sub->address, array[j]);
   }
   
 }
 head->submitter = sub;
 object = calloc(1,sizeof(GEDCOMobject));
 object->header = head;
 object->submitter = sub;
 
 printf("source: %s, vers: %.2lf, submitter name: %s, address: %s\n", object->header->source, object->header->gedcVersion, object->header->submitter->submitterName, object->submitter->address);
 return object;
}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded){
  if (obj == NULL || toBeAdded == NULL){
    return ;
  }
  //Individual * adding = (Individual*)toBeAdded;
  insertBack(&obj->individuals, (void*)toBeAdded);
}

char* iListToJSON(List iList){
  char * string = NULL;
  if (iList.head == NULL){
    string = calloc(10, sizeof(char));
    strcpy(string, "[]");
    return string;
  }
  Node * person = iList.head;
  int len = 0;
  string = calloc(10,sizeof(char));
  sprintf(string, "[");
  while(person != NULL){
    Individual * current = (Individual*)person->data;
    char * temp = indToJSON(current);
    len = strlen(temp) + len;
    //printf("before realloc\n");
    string = realloc(string, sizeof(char) * (len + 50));
    //printf("after realloc\n");
    //printf("address %p\n", (void*)current);
    //char * temp = indToJSON(current);
    if (temp != NULL){
      strcat(string, temp);
      strcat(string, ",");
    }
    //strcat(string, indToJSON(current));
    //strcat(string, ",")
    free(temp);
    person = person->next;
  }
  string[strlen(string) - 1] = '\0';
  strcat(string, "]");
  return string;
}

char* gListToJSON(List gList){
  char * string = NULL;
  if (gList.head == NULL){
    string = calloc(10, sizeof(char));
    strcpy(string, "[]");
    return string;
  }
  string = calloc(10000,sizeof(char));
  sprintf(string, "[");
  /*Node * lists = gList.head;
  while(lists != NULL){
    List * list = (List*)gList.head->data;
    char* tempStr = iListToJSON(*list);
    strcat(string, tempStr);
    free(tempStr);
    strcat(string, ",");
    lists = lists->next;
  }*/
  Node * Big = gList.head;
  for(int i = 0; i < gList.length; i++){
	  List * list = (List*)Big->data;
	  char * temp = iListToJSON(*list);
	  strcat(string, temp);
	  free(temp);
	  strcat(string, ",");
	  //puts(temp);
	  //free(temp);
	  Big = Big->next;
  }
  string[strlen(string) - 1] = '\0';
  strcat(string, "]");
  return string;
}

//************************************************************************************************************

void deleteGeneration(void* toBeDeleted){
	List * Delete;
	if (toBeDeleted == NULL){
		return ;
	}
	Delete = (List*)toBeDeleted;
	/*Node * current = Delete->head;
	while(current != NULL){
		List * indi = (List*)current->data;
		clearList(indi);
		current = current->next;
	}*/
	clearList(Delete);
	free(Delete);
}
	
int compareGenerations(const void* first,const void* second){
	return 0;
}


char* printGeneration(void * toBePrinted){
	char * string = NULL;
	
	if (toBePrinted == NULL){
		return string;
	}
	List * print = (List*)toBePrinted;
	string =  toString(*print);
	
	return string;
	
	
}

//****************************************** List helper functions *******************************************
void deleteEvent(void* toBeDeleted){
  Event * Delete;

  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Event*)toBeDeleted;

  free(Delete->date);
  free(Delete->place);
  clearList(&Delete->otherFields);
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
char* printEvent(void* toBePrinted){//type date place other fields
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
  chrTemp = (char*)calloc(100000, sizeof(char));
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
  clearList(&Delete->events);
  clearList(&Delete->families);
  clearList(&Delete->otherFields);
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
  
  if (strlen(compare1->surname) == 0 || strlen(compare2->surname) == 0){
    return 0;//if they have no last name // must go to the end of the list
  }
  if (strcmp(compare1->surname, compare2->surname) == 0){
    return strcmp(compare1->givenName, compare2->givenName);
  }
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
  //printf("in here\n");
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
  string = (char*)malloc(100000 * sizeof(char));
  sprintf(string, "FirstName: %s, LastName: %s\n", print->givenName, print->surname);
  return string;
}

void deleteFamily(void* toBeDeleted){
  Family * Delete;

  if (toBeDeleted == NULL){
    return;
  }
  Delete = (Family*)toBeDeleted;
  clearList(&Delete->children);
  clearList(&Delete->events);
  clearList(&Delete->otherFields);
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
