#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedListAPI.h"

#define DEBUG true


GEDCOMerror setType(ErrorCode code, int line);

void toLower(char * toBeLower);

enum eCode validateFile(char* fileName);

enum eCode validateGEDCOM(char * fileName);

char** fileReader(char * fileName);

int fileLength(char ** array);

int checkTerminate(char * string);
