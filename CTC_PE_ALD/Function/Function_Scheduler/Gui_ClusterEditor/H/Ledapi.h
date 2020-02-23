#ifndef LEDAPI_H
#define LEDAPI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* - - - - - - - - - - - -   D E F I N E S    - - - - - - - - - - - - - - */

#define CDB_ADDR void *

void All2FileName( char * , char * , int );

void Kor_CLUSTER_Initial();
BOOL Kor_CLUSTER_Open( char * , char * );
BOOL Kor_CLUSTER_Save( char * , int , char * );
BOOL Kor_CLUSTER_Delete( char* RecipeFileName , int renamemode , char *groupstring );

#endif