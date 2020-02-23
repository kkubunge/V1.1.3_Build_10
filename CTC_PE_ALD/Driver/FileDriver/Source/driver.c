#include <stdio.h>
#include "CimRs232.h"
#include "driver.h"
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
typedef enum  {TYPE_DIGITAL = 0 , TYPE_ANALOG , TYPE_STRING} IO_TYPE;
typedef struct sIOMem{
	//IO Key Value
	int     nIOKeyValue;
	//IO Struct
	IO_TYPE enIOType;
	int     nValue;
	double  dbValue;
	char    szValue[256];
	//List
	struct sIOMem* psNext;
}IO_MEM;
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
IO_MEM* gp_IOStart  = NULL;
IO_MEM* gp_IOLast   = NULL;
int     g_nIOCount  = 0;
FILE*   g_fIOMem    = NULL;
char    g_szDrvName[64]; 

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL IsValidIO(int nKey) {
	BOOL bRet       = TRUE;
	IO_MEM* psIO    = NULL;

	psIO = gp_IOStart;
	while (NULL != psIO) {
		if (psIO->nIOKeyValue == nKey) {bRet = FALSE; break;}
		psIO = psIO->psNext;
	}

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
IO_MEM* FindIO(int nKey) {
	IO_MEM* psIO    = NULL;
	IO_MEM* psIORet = NULL;

	psIO = gp_IOStart;
	while (NULL != psIO) {
		if (psIO->nIOKeyValue == nKey) {psIORet = psIO; break;}
		psIO = psIO->psNext;
	}

	return psIORet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL AddIO(IO_MEM* psIO) {
	BOOL bRet = TRUE;

	if (NULL == psIO)      return FALSE;
	if (NULL == gp_IOStart) {
		gp_IOStart        = psIO;
		gp_IOLast         = gp_IOStart;
		gp_IOLast->psNext = NULL;
	} else {
		gp_IOLast->psNext = psIO;
		gp_IOLast         = gp_IOLast->psNext;
		gp_IOLast->psNext = NULL;
		g_nIOCount++;
	}

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void RemovAllList() {
	IO_MEM* psIO   = NULL;
	IO_MEM* psFree = NULL;

	psIO = gp_IOStart;
	while (NULL != psIO) {
		psFree = psIO;
		psIO   = psIO->psNext;
		free(psFree);
	}
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL SaveIOFile() {
	BOOL bRet      = FALSE;
	IO_MEM* psIO   = NULL;
	SYSTEMTIME       systime;

	//File Save
	g_fIOMem = fopen(g_szDrvName , "w+");
	if (NULL != g_fIOMem)
		printf("Save IO File Start... \n");
	else return FALSE;

	GetLocalTime(&systime);
	fprintf(g_fIOMem , "#============================================================================\n");
	fprintf(g_fIOMem , "#Key(ID1)\t\tIOType\t\tValue       Last Save : %d.%2d.%2d.%2d.%2d.%2d\n" , systime.wYear , systime.wMonth , systime.wDay , systime.wHour , systime.wMinute , systime.wSecond);
	fprintf(g_fIOMem , "#============================================================================\n");
	psIO = gp_IOStart;
	while (NULL != psIO) {
		switch (psIO->enIOType) {
		case TYPE_DIGITAL:
			fprintf(g_fIOMem , "%d\t\t%d\t\t%d\n"   , psIO->nIOKeyValue , psIO->enIOType , psIO->nValue);
			break;
		case TYPE_ANALOG:
			fprintf(g_fIOMem , "%d\t\t%d\t\t%.1f\n" , psIO->nIOKeyValue , psIO->enIOType , psIO->dbValue);
			break;
		case TYPE_STRING:
			fprintf(g_fIOMem , "%d\t\t%d\t\t%s\n"   , psIO->nIOKeyValue , psIO->enIOType , psIO->szValue);
			break;
		default: break;
		}
		psIO = psIO->psNext;
	}
	
	fclose(g_fIOMem);
	printf("Save IO File Completed... \n");
	bRet = TRUE;
	
	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL InitIOFile(FILE* IOFile) {
	BOOL    bRet = TRUE;
	char    szRead[256];
	char*   szTemp     = NULL;
	char    szTok[]    = "\r\n\t";
	int     nKeyValue  = 0;
	IO_TYPE enType     = TYPE_DIGITAL;
	int     nValue     = 0;
	double  dbValue    = 0.0f;

	if (NULL == fgets(szRead, 256, IOFile)) {printf("File is Empty \n");return FALSE;}
	do {
		if (szRead[0] == ';' || szRead[0] == '#' || szRead[0] == '\r' || szRead[0] == '\n' ) {/*do nothing*/}
		else {
			//Save Value from file to struct
			szTemp    = strtok(szRead , szTok);
			nKeyValue = atoi(szTemp);
			szTemp    = strtok(NULL , szTok);
			enType    = (IO_TYPE)atoi(szTemp);
			szTemp    = strtok(NULL , szTok);
			if (nKeyValue > 0) {
				IO_MEM* psIO = NULL;
				psIO = malloc(sizeof(IO_MEM));	
				psIO->nIOKeyValue = nKeyValue;
				psIO->enIOType    = enType;
				switch (enType) {
				case TYPE_DIGITAL:
					nValue = atoi(szTemp); 
					psIO->nValue = nValue; psIO->dbValue = 0.0f; memset(psIO->szValue , 0 , sizeof(psIO->szValue)); break;
				case TYPE_ANALOG:
					dbValue = atof(szTemp);   
					psIO->dbValue = dbValue; psIO->nValue = 0;    memset(psIO->szValue , 0 , sizeof(psIO->szValue)); break;
				case TYPE_STRING:
					if (NULL == szTemp) memset(psIO->szValue , 0 , sizeof(psIO->szValue));
					else {strcpy(psIO->szValue , szTemp); }
					psIO->nValue = 0; psIO->dbValue = 0.0f; break;
				default: return FALSE;
				}	
				//Construct & Link List
				if (NULL == gp_IOStart) {
					gp_IOStart        = psIO;
					gp_IOLast         = gp_IOStart;
					gp_IOLast->psNext = NULL;
				} else {
					gp_IOLast->psNext = psIO;
					gp_IOLast         = gp_IOLast->psNext;
					gp_IOLast->psNext = NULL;
				}
				g_nIOCount++;
			} else return FALSE;
					
		}
		
		if (NULL == fgets(szRead, 256, IOFile)) break;
	} while (feof(IOFile) == 0);

	return bRet;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

    sprintf(g_szDrvName , ".\\Parameter\\ChamberContionControl\\%s.mem" , Name);
    g_fIOMem = fopen(g_szDrvName , "rt");
	if (NULL != g_fIOMem) {
		printf("Start IO File Access For Initialize IO Value \n");
		if   (FALSE == InitIOFile(g_fIOMem)) {fclose(g_fIOMem); return FALSE;}
		else fclose(g_fIOMem);
		printf("Complete IO File Access For Initialize IO Value \n");
	} else {
		printf("IO File Not Yet Exist \n");
	}

	return TRUE;	
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {

	//2013.10.26 Delete SaveIOFile() because File Driver Crash...
	//if (FALSE == SaveIOFile()) return FALSE;

	//Free Allocated Memory
	printf("Free All Allocated Memory Start.. \n");
	RemovAllList();
	printf("Free All Allocated Memory Completed.. \n");
	return TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	IO_MEM* psIO = NULL;
	if (ID1 <= 0) {*Result = FALSE; return 0.0f;}

	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n" , ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE;printf ("IO Key %d is not valid key so Add fail...\n" , ID1); return 0.0f;}
		psIO              = malloc(sizeof(IO_MEM));
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_ANALOG;
		psIO->dbValue     = 0.0f; psIO->nValue = 0; memset(psIO->szValue , 0 , sizeof(psIO->szValue));
		if (FALSE == AddIO(psIO)) {*Result = FALSE; printf ("Add IO Fail about key %d...\n" , ID1);return 0.0f;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType = TYPE_ANALOG;
		*Result        = TRUE;
		return psIO->dbValue;
	}
	
	*Result = TRUE;
	return 0.0f;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Out( int ID1 , int ID2 , int ID3 , double data , int *Result ) {
    IO_MEM* psIO = NULL;
	BOOL    bSave= FALSE;

	if (ID1 <= 0) {*Result = FALSE; return;}

	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n" , ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE;printf ("IO Key %d is not valid key so Add fail...\n" , ID1); return;}
		psIO              = malloc(sizeof(IO_MEM));
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_ANALOG;
		psIO->dbValue     = data; 
		psIO->nValue      = 0; memset(psIO->szValue , 0 , sizeof(psIO->szValue));
		if (FALSE == AddIO(psIO)) {*Result = FALSE; printf ("Add IO Fail about key %d...\n" , ID1);return;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType    = TYPE_ANALOG;
		if (psIO->dbValue != data) {psIO->dbValue = data; bSave = TRUE;}
	}

	//Save new write value
	if (TRUE == bSave) {
		if (FALSE == SaveIOFile()) {*Result = FALSE; printf ("Save New Write Value key %d... is failed\n" , ID1);return;}
	}

	*Result = TRUE;
	return;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	IO_MEM* psIO = NULL;
	if (ID1 <= 0) {*Result = FALSE; return 0;}
   
	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n" , ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE; printf ("IO Key %d is not valid key so Add fail...\n" , ID1);return 0;}
		psIO              = malloc(sizeof(IO_MEM));
		printf("Key %d , New IO Point %d \n" , ID1 , psIO);
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_DIGITAL;
		psIO->dbValue     = 0.0f; psIO->nValue = 0; memset(psIO->szValue , 0 , sizeof(psIO->szValue));
		if (FALSE == AddIO(psIO)) {*Result = FALSE;printf ("Add IO Fail about key %d...\n" , ID1); return 0;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType    = TYPE_DIGITAL;
		*Result = TRUE;
		return psIO->nValue;
	}

	*Result = TRUE;	
	return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int data , int *Result ) {
    IO_MEM* psIO = NULL;
	BOOL    bSave= FALSE;

	if (ID1 <= 0) {*Result = FALSE; return;}

	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n" , ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE; printf ("IO Key %d is not valid key so Add fail...\n" , ID1);return;}
		psIO              = malloc(sizeof(IO_MEM));
		printf("Key %d , New IO Point %d \n" , ID1 , psIO);
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_DIGITAL;
		psIO->nValue      = data;
		psIO->dbValue     = 0.0f; memset(psIO->szValue , 0 , sizeof(psIO->szValue));		
		if (FALSE == AddIO(psIO)) {*Result = FALSE; printf ("Add IO Fail about key %d...\n" , ID1);return;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType    = TYPE_DIGITAL;
		if (psIO->nValue != data) {psIO->nValue = data; bSave = TRUE;}
	}

	//Save new write value
	if (TRUE == bSave) {
		if (FALSE == SaveIOFile()) {*Result = FALSE; printf ("Save New Write Value key %d... is failed\n" , ID1);return;}
	}

	*Result = TRUE;
	return;

}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_In( int ID1 , char *data , int *Result ) {
	IO_MEM* psIO = NULL;
	if (ID1 <= 0) {*Result = FALSE; return;}

	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n" , ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE; printf ("IO Key %d is not valid key so Add fail...\n", ID1);return;}
		psIO              = malloc(sizeof(IO_MEM));
		printf("Key %d , New IO Point %d \n" , ID1 , psIO);
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_STRING;
		psIO->nValue      = 0; psIO->dbValue = 0.0f; memset(psIO->szValue , 0 , sizeof(psIO->szValue));		
		if (FALSE == AddIO(psIO)) {*Result = FALSE; printf ("Add IO Fail about key %d...\n" , ID1);return;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType    = TYPE_STRING;
		strcpy(data , psIO->szValue);
	}

	*Result = TRUE;
	return;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Out( int ID1, char *data , int *Result ) {
	IO_MEM* psIO = NULL;
	BOOL    bSave= FALSE;

	if (ID1 <= 0)           {*Result = FALSE; return;}
	if (strlen(data) > 255) {*Result = FALSE; return;}

	psIO = FindIO(ID1);
	if (NULL == psIO) {
		printf ("Find IO Key %d Fail so AddIO Now...\n", ID1);
		if (FALSE == IsValidIO(ID1)) {*Result = FALSE; printf ("IO Key %d is not valid key so Add fail...\n", ID1);return;}
		psIO              = malloc(sizeof(IO_MEM));
		printf("Key %d , New IO Point %d \n" , ID1 , psIO);
		psIO->nIOKeyValue = ID1;
		psIO->enIOType    = TYPE_STRING;
		strcpy(psIO->szValue , data);
		psIO->nValue      = 0; psIO->dbValue = 0.0f;		
		printf("Add String IO Key %d , Value %s \n" , ID1 , data);
		if (FALSE == AddIO(psIO)) {*Result = FALSE; printf ("Add IO Fail about key %d...\n" , ID1);return;}		
	} else {
		//IO_Type Confirm : some case Key ID can be changed so IO Type has possibility of change from original
		psIO->enIOType    = TYPE_STRING;
		if (0 != strcmp(psIO->szValue , data)) {strcpy(psIO->szValue , data); bSave = TRUE;}
	}

	//Save new write value
	if (TRUE == bSave) {
		if (FALSE == SaveIOFile()) {*Result = FALSE; printf ("Save New Write Value key %d... is failed\n" , ID1);return;}
	}

	*Result = TRUE;
	return;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
double Analog_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Analog_Name_Out( char *Name , double data , int *Result ) {
	*Result = TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void Digital_Name_Out( char *Name , int data , int *Result ) {
	*Result = TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_In( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

