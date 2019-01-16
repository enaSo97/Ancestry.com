#ifndef A3HELPER
#define A3HELPER
#include <stdlib.h>



char * FileInfo(char * file);

char * printCharSet(CharSet code);

char * indiMoreInfo(Individual * ind);

int addIndiWrapper(char * file, char * first, char * last);

Individual * createPerson(char * first, char * last);

char * moreIList(List list);

int createSimpleGEDCOM(char * file, char * source, char * vers, char * encode, char * submitter, char * address);

char * returnDescendents(char * file, char * first, char * last, int maxGen);

#endif
