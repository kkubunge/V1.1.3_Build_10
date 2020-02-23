#include "iodefine.h"
#include "file.h"
#include "Prg_Param.h"
#include "Data_type.h"
#include "LotType.h"
//------------------------------------------------------------------------------------------

RecipeStepTemplate 	F_RECIPE0;
RecipeStepTemplateM	F_RECIPE[ MAX_STEP_COUNT ];
RecipeStepTemplateM	F_RECIPE_BUFFER;
BOOL				F_RECIPE_BUFFER_STATUS = FALSE ;
int					F_STEP_COUNT	=	0;

//------------------------------------------------------------------------------------------
// 2006.01.02
extern double GetStepTime(RecipeStepTemplateM* pRcpStep);
double GetProcTotalTime()
{
	double dbProcTotalTime = 0;
	int i, j;
	for(i=0; i<F_STEP_COUNT; i++)
	{
		dbProcTotalTime += GetStepTime(& F_RECIPE[i]);
	}

	do {
		if(F_RECIPE0.nStepGrpCyc <= 1) break;
		if(F_RECIPE0.nStepGrpBegin == 0)  break;
		if(F_RECIPE0.nStepGrpEnd == 0) break;
		if(F_RECIPE0.nStepGrpBegin > F_STEP_COUNT) { dbProcTotalTime = 0; break; }
		if(F_RECIPE0.nStepGrpEnd > F_STEP_COUNT) { dbProcTotalTime = 0; break; }
		if(F_RECIPE0.nStepGrpEnd < F_RECIPE0.nStepGrpBegin) { dbProcTotalTime = 0; break; }

		for(j=0; j<F_RECIPE0.nStepGrpCyc-1; j++)
		{
			for(i=F_RECIPE0.nStepGrpBegin-1; i<F_RECIPE0.nStepGrpEnd; i++)
			{
				dbProcTotalTime += GetStepTime(& F_RECIPE[i]);
			}
		}
	} while(0);
	return dbProcTotalTime;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Fixed Area for Progarm
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


void Kor_All2FileName( char *Target , const char *Source , int MaxCnt ) 
{
	int pnt , i , j;
	j = (int) strlen( Source );
	for ( pnt = j ; pnt > 0 ; pnt-- ) if ( Source[ pnt-1 ] == '/' || Source[ pnt-1 ] == '\\' ) break;
	for ( i = pnt ; ( i < j ) && ( i-pnt < MaxCnt ); i++ ) Target[ i-pnt ] = Source[ i ];
	Target[ i-pnt ] = 0x00;
}
//------------------------------------------------------------------------------------------
void Kor_Recipe_Initial() 
{
	int i;
	F_STEP_COUNT = 0;
	Kor_RecipeStep0_Initial();
	for ( i = 0 ; i < MAX_STEP_COUNT ; i++ ) Kor_RecipeStep_Initial( i );
}
//------------------------------------------------------------------------------------------
void Kor_Step0DataGet( RECIPE_ADDR RecipeStep ) 
{
	memcpy(	RecipeStep , &F_RECIPE0 , sizeof( F_RECIPE0 ) );
}
//------------------------------------------------------------------------------------------
void Kor_Step0DataPut( RECIPE_ADDR RecipeStep ) 
{
	memcpy(	&F_RECIPE0 , RecipeStep , sizeof( F_RECIPE0 ) );
}
//------------------------------------------------------------------------------------------
void Kor_StepDataGet( int StepID , RECIPE_ADDR RecipeStep ) 
{
	if ( StepID < 1 || StepID >= MAX_STEP_COUNT ) return;
	memcpy(	RecipeStep , &F_RECIPE[ StepID-1 ] , sizeof( F_RECIPE[ StepID-1 ] ) );
}
//------------------------------------------------------------------------------------------
void Kor_StepDataPut( int StepID , RECIPE_ADDR RecipeStep ) 
{
	if ( StepID < 1 || StepID >= MAX_STEP_COUNT ) return;
	memcpy(	&F_RECIPE[ StepID-1 ] , RecipeStep , sizeof( F_RECIPE[ StepID-1 ] ) );
}
//------------------------------------------------------------------------------------------
void Kor_StepDelete( int StepID ) 
{
	int i;
	if ( StepID < 1 || StepID >= MAX_STEP_COUNT ) return;
	if ( F_STEP_COUNT < 1 ) return;
	for ( i = ( StepID - 1 ) ; i < F_STEP_COUNT ; i++ ) {
		memcpy(	&F_RECIPE[ i ] , &F_RECIPE[ i+1 ] , sizeof( F_RECIPE[ StepID-1 ] ) );
	}
	F_STEP_COUNT--;
}
//------------------------------------------------------------------------------------------
void Kor_StepInsert( int StepID , BOOL Init ) 
{
	int i;
	if ( StepID < 1 || StepID >= MAX_STEP_COUNT ) return;
	if ( F_STEP_COUNT >= MAX_STEP_COUNT ) return;
	for ( i = F_STEP_COUNT ; i > ( StepID - 1 ) ; i-- ) {
		memcpy(	&F_RECIPE[ i ] , &F_RECIPE[ i-1 ] , sizeof( F_RECIPE[ i ] ) );
	}
	if ( Init || !F_RECIPE_BUFFER_STATUS ) {
		Kor_RecipeStep_Initial( StepID-1 );
	}
	else {
		memcpy(	&F_RECIPE[ StepID-1 ] , &F_RECIPE_BUFFER , sizeof( F_RECIPE_BUFFER ) );
	}
	F_STEP_COUNT++;
}
//------------------------------------------------------------------------------------------
int  Kor_StepCount() 
{
	return F_STEP_COUNT;
}
//------------------------------------------------------------------------------------------
void Kor_BufferDataPut( RECIPE_ADDR RecipeStep ) 
{
	F_RECIPE_BUFFER_STATUS = TRUE;
	memcpy(	&F_RECIPE_BUFFER , RecipeStep , sizeof( F_RECIPE_BUFFER ) );
}
//------------------------------------------------------------------------------------------
BOOL Kor_BufferDataStatus() 
{
	return F_RECIPE_BUFFER_STATUS;
}
//------------------------------------------------------------------------------------------
// 2006.01.02
void PutGrpStepCycleData(RecipeStepTemplate *pRcpHeader)
{
	F_RECIPE0.nStepGrpBegin = pRcpHeader->nStepGrpBegin;
	F_RECIPE0.nStepGrpEnd = pRcpHeader->nStepGrpEnd;
	F_RECIPE0.nStepGrpCyc = pRcpHeader->nStepGrpCyc;
}