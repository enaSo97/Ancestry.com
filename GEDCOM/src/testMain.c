#include <stdio.h>
#include <stdlib.h>
#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
#include "LinkedListAPI.h"

int main (int argc, char ** argv){
  printf("This is test main\n");
  //char reader[1000];
  //FILE * file = fopen(argv[1], "r");

  GEDCOMobject* object;
  GEDCOMerror error;

  if(DEBUG)printf("Calling the object\n");
  error = createGEDCOM(argv[1],&object);
  if(DEBUG)printf("obejct has been called\n");

  //Individual * person = object->individuals.head->data;
  //printFamily(person->families.head->data);
  //printf("families of last inividual\n");
  //puts(printFamily(person->families.head->data));
  //printf("-----------------------------------\n");
  //puts(printFamily(person->families.head->next->data))
  puts(printGEDCOM(object));
  //puts(toString(object->individuals));
  //puts(printFamily(object->families.head->data));
  //puts(printError(error));
  
  return 0;
}
