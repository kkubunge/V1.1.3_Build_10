#ifndef FILE_H
#define FILE_H

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"

#define		RECIPE_ADDR	void *

#define		MAX_STEP_COUNT			122 //2005.10.28 by cskim, 122 Step Possible
#define		MAX_DISPLAY_COUNT		2

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Kor_Recipe_Initial();
//------------------------------------------------------------------------------------------
BOOL Kor_Recipe_Open( char * );
BOOL Kor_Recipe_Save( char * );
//------------------------------------------------------------------------------------------
void Kor_RecipeStep0_Initial();
void Kor_RecipeStep_Initial( int i );
//------------------------------------------------------------------------------------------
void Kor_Recipe_Initial();
//------------------------------------------------------------------------------------------
void Kor_All2FileName( char * , const char * , int );
//------------------------------------------------------------------------------------------
void Kor_Step0DataGet( RECIPE_ADDR RecipeStep );
void Kor_Step0DataPut( RECIPE_ADDR RecipeStep );
void Kor_StepDataGet( int StepID , RECIPE_ADDR RecipeStep );
void Kor_StepDataPut( int StepID , RECIPE_ADDR RecipeStep );
void Kor_StepDelete( int StepID );
void Kor_StepInsert( int , BOOL  );
int  Kor_StepCount();
void Kor_BufferDataPut( RECIPE_ADDR RecipeStep );
BOOL Kor_BufferDataStatus();
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif