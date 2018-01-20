#include <stdio.h>
#include <stdlib.h>
#include "GEDCOMparser.h"
#include "GEDCOMhelper.h"
#include "LinkedListAPI.h"

int main (int argc, char ** argv){
  printf("This is test main\n");
  //char reader[1000];
  FILE * file = fopen(argv[1], "r");

  GEDCOMobject* object;

  if(DEBUG)printf("Calling the object\n");
  createGEDCOM(argv[1],&object);
  if(DEBUG)printf("obejct has been called\n");

  fclose(file);

  return 0;
}
