#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"
#include "A3helper.h"
#include "GEDCOMutilities.h"

char * FileInfo(char * file){
	char * string = NULL;
	
	GEDCOMobject * object;
	GEDCOMerror error = createGEDCOM(file, &object);
	if (error.type != OK){
		return "Fail";
	}
	Header * head = object->header;
	
	int len = strlen(head->source);
	
	string = calloc(len+999, sizeof(char));
	sprintf(string, "{\"source\":\"");
	strcat(string, head->source);
	strcat(string, "\",\"gedcVersion\":\"");
	
	char * number = calloc(10, sizeof(char));
	
	sprintf(number, "%lf", head->gedcVersion);
	
	strcat(string, number);
	strcat(string, "\",\"encoding\":\"");
	strcat(string, printCharSet(head->encoding));
	strcat(string, "\",\"submitterName\":\"");
	strcat(string, head->submitter->submitterName);
	//if (head->submitter)
	if (head->submitter->address != NULL){
		if (strlen(head->submitter->address) > 0){
			strcat(string, "\",\"address\":\"");
			strcat(string, head->submitter->address);
		}
		else {
			strcat(string, "\",\"address\":\"");
		}
	}
	char indiNum[40];
	sprintf(indiNum, "\",\"individuals\":\"%d\"", object->individuals.length);
	strcat(string, indiNum);
	char famNum[40];
	sprintf(famNum, ",\"families\":\"%d\"", object->families.length);
	strcat(string, famNum);
	strcat(string, "}");
	printf("%s\n", string);
	
	deleteGEDCOM(object);
	//int addToFile(char * file);
	
	return string;
}
/**
 * return 0 if the indi is aded to file safely
 * **/
int createSimpleGEDCOM(char * file, char * source, char * vers, char * encode, char * submitter, char * address){
	char temp[250];
	printf("in the function\n");
	//int len = strlen(file) + strlen(source) + strlen(vers) + strlen(encode) + strlen(submitter);
	printf("what?\n");
	char * string = calloc(999, sizeof(char));
	printf("size given\n");
	if (address == NULL){
		//printf("address NULL\n");
		strcpy(temp, "");
	}
	else if (address != NULL){
		//printf("address not null\n");
		if (strlen(address) > 0){
			strcpy(temp, address);
		}else{
			strcpy(temp, "");
		}
	}
	//printf("before makeing string\n");
	sprintf(string, "{\"source\":\"%s\",\"gedcVersion\":\"%s\",\"encoding\":\"%s\",\"subName\":\"%s\",\"subAddress\":\"%s\"}", 
		source, vers, encode, submitter, address);
	//printf("string: %s", string);
	GEDCOMobject * obj = JSONtoGEDCOM(string);
	GEDCOMerror error = writeGEDCOM(file, obj);
	deleteGEDCOM(obj);
	if(error.type == WRITE_ERROR){
		return 1;
	}
	return 0;
}

/**
 * return 0 if the indi is aded to file safely
 * **/
int addIndiWrapper(char * file, char * first, char * last){
	//int result = 0;
	GEDCOMobject * object;
	GEDCOMerror check;
	createGEDCOM(file, &object);
	printf("%s %s %s\n", file, first, last);
	Individual * person = createPerson(first, last);
	char * indi = printIndividual(person);
	puts(indi);
	addIndividual(object, person);
	check = writeGEDCOM(file, object);
	if (check.type == OK){
		printf("Success write\n");
	}
	//deleteGEDCOM(object);
	printf("before return\n");
	return 0;
}

Individual * createPerson(char * first, char * last){
	Individual * person = calloc(1, sizeof(Individual));
	person->givenName = calloc(30,sizeof(char));
	person->surname = calloc(30, sizeof(char));
	if (first == NULL){
		strcpy(person->givenName, "");
	}else{
		strcpy(person->givenName, first);
	}
	//person->surname = malloc(sizeof(char)*(strlen(surname)+1));
	if (last == NULL){
		strcpy(person->surname, "");
	}else{
		strcpy(person->surname, last);
	}
	person->events = initializeList(printEvent, deleteEvent, compareEvents);
	person->families = initializeList(printFamily, deleteFamily, compareFamilies);
	person->otherFields = initializeList(printField, deleteField, compareFields);
	return person;
}

char * getIndi(char * file){
	char * string = NULL;
	GEDCOMobject * object;
	createGEDCOM(file, &object);
	printf("get indi\n");
	string = moreIList(object->individuals);
	deleteGEDCOM(object);
	return string;
}

char * indiMoreInfo(Individual * ind){
	printf("ine here\n");
	char * string = "";
	string = calloc(10000, sizeof(char));
	char value[5];
	int sex = 0;
	if (ind == NULL){
		strcpy(string, "");
		return string;
	}

	else if (ind->givenName == NULL){
		Node * other = ind->otherFields.head;
		Node * fam = ind->families.head;
		int famCount = 1;
		while(fam != NULL){
			Family * one = (Family*)fam->data;
			if (one->husband == ind || one->wife == ind){
				famCount += one->children.length + 1;
			}
			fam = fam->next;
		}
		while(other != NULL){
			Field * data = (Field*)other->data;
			if (strcmp(data->tag, "SEX") == 0){
				strcpy(value, data->value);
				//printf("sex exist\n");
				sex = 1;
			}
			other = other->next;
		}
		if (sex == 1){
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", "", ind->surname, value, famCount);
		}else{
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", "", ind->surname, "N/A", famCount);
		}
	}
	else if (ind->surname == NULL){
		sex = 0;
		Node * other = ind->otherFields.head;
		int famCount = 1;
		Node * fam = ind->families.head;
		while(fam != NULL){
			Family * one = (Family*)fam->data;
			if (one->husband == ind || one->wife == ind){
				famCount += one->children.length + 1;
			}
			fam = fam->next;
		}
		while(other != NULL){
			Field * data = (Field*)other->data;
			if (strcmp(data->tag, "SEX") == 0){
				strcpy(value, data->value);
				//printf("sex exist\n");
				sex = 1;
			}
			other = other->next;
		}
		if (sex == 1){
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", ind->givenName, "", value, famCount);
		}else{
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", ind->givenName, "", "N/A", famCount);
		}
	}
	else{
		sex = 0;
		Node * other = ind->otherFields.head;
		int famCount = 1;
		Node * fam = ind->families.head;
		while(fam != NULL){
			Family * one = (Family*)fam->data;
			if (one->husband == ind || one->wife == ind){
				famCount += one->children.length + 1;
			}
			fam = fam->next;
		}
		while(other != NULL){
			Field * data = (Field*)other->data;
			if (strcmp(data->tag, "SEX") == 0){
				strcpy(value, data->value);
				sex = 1;
			}
			other = other->next;
		}
		if (sex == 1){
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", ind->givenName, ind->surname, value, famCount);
		}else{
			sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"sex\":\"%s\",\"families\":\"%d\"}", ind->givenName, ind->surname, "N/A", famCount);
		}
		printf("string %s\n", string);
			//sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", ind->givenName, ind->surname);
	}
	return string;
}
char * moreIList(List iList){
  char * string = NULL;
  printf("find me\n");
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
    char * temp = indiMoreInfo(current);
    len = strlen(temp) + len;

    string = realloc(string, sizeof(char) * (len + 50));

    if (temp != NULL){
      strcat(string, temp);
      strcat(string, ",");
    }

    free(temp);
    person = person->next;
  }
  string[strlen(string) - 1] = '\0';
  strcat(string, "]");
  return string;
}

char * printCharSet(CharSet code){
	char * string = calloc(10, sizeof(char));
	
	if (code == ASCII){
		sprintf(string, "ASCII");
	}
	else if (code == UNICODE){
		sprintf(string, "UNICODE");
	}
	else if (code == UTF8 ){
		sprintf(string, "UTF-8");
	}
	else if(code == ANSEL){
		sprintf(string, "ANSEL");
	}
	return string;
}

char * returnAncestors(char * file, char * first, char * last, int maxGen){
	Individual * dummy = calloc(1, sizeof(Individual));
	GEDCOMobject * object;
	GEDCOMerror error = createGEDCOM(file, &object);
	if (error.type != OK){
		return "fail";
	}
	List result;
	
	dummy->givenName = calloc(40,sizeof(char));
	dummy->surname = calloc(40,sizeof(char));
	
	strcpy(dummy->givenName, first);
	strcpy(dummy->surname, last);
	
	result = getAncestorListN(object, dummy, maxGen);
	
	char * string = gListToJSON(result);
	puts(string);
	
	return string;
}

char * returnDescendents(char * file, char * first, char * last, int maxGen){
	Individual * dummy = calloc(1, sizeof(Individual));
	GEDCOMobject * object;
	GEDCOMerror error = createGEDCOM(file, &object);
	if (error.type != OK){
		return "fail";
	}
	List result;
	
	dummy->givenName = calloc(40,sizeof(char));
	dummy->surname = calloc(40,sizeof(char));
	
	strcpy(dummy->givenName, first);
	strcpy(dummy->surname, last);
	
	result = getDescendantListN(object, dummy, maxGen);
	
	char * string = gListToJSON(result);
	puts(string);
	
	return string;
}
