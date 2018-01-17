#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
  FILE * file = fopen(fileName, "r");

  if (file == NULL || strcmp(fileName, "") == 0){
    setType(INV_FILE, 0);
    return
  }

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
void deleteEvent(void* toBeDeleted);
int compareEvents(const void* first,const void* second);
char* printEvent(void* toBePrinted);

void deleteIndividual(void* toBeDeleted);
int compareIndividuals(const void* first,const void* second);
char* printIndividual(void* toBePrinted);

void deleteFamily(void* toBeDeleted);
int compareFamilies(const void* first,const void* second);
char* printFamily(void* toBePrinted);

void deleteField(void* toBeDeleted);
int compareFields(const void* first,const void* second);
char* printField(void* toBePrinted);
//************************************************************************************************************
