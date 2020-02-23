
#include "stdafx.h"
#include "TextParser.h"
#include "EthernetManager.h"
#include "PortInfo.h"
#include "WindowInterface.h"
//------------------------------------------------------------------------------------------
// Initialize Function
#define PORT_INFO_FILE		".\\Function\\Function_IO_Monitor_CTC\\Info\\PortInfo.ini"

CWindowInterface*  g_pWindowInterface = NULL;
CEthernetManager*  g_pPM1Manager  = NULL;
CEthernetManager*  g_pPM2Manager  = NULL;
CEthernetManager*  g_pPM3Manager  = NULL;
CEthernetManager*  g_pPM4Manager  = NULL;
CEthernetManager*  g_pPM5Manager  = NULL;
CEthernetManager*  g_pPM6Manager  = NULL;
CPortInfo*         g_pPM1Portnfo = NULL;
CPortInfo*         g_pPM2Portnfo = NULL;
CPortInfo*         g_pPM3Portnfo = NULL;
CPortInfo*         g_pPM4Portnfo = NULL;
CPortInfo*         g_pPM5Portnfo = NULL;
CPortInfo*         g_pPM6Portnfo = NULL;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};

extern "C" void Screen_Nav_Info( int no )
{
}
extern "C" BOOL Program_Init_Code()
{	
	return TRUE;
}
extern "C" void Program_Enter_Code()
{	
}
extern "C" void Event_Message_Received()
{	
}

void DeInitializeMain()
{
	if (NULL != g_pWindowInterface) delete g_pWindowInterface;
	if (NULL != g_pPM1Portnfo) delete g_pPM1Portnfo;
	if (NULL != g_pPM2Portnfo) delete g_pPM2Portnfo;
	if (NULL != g_pPM3Portnfo) delete g_pPM3Portnfo;
	if (NULL != g_pPM4Portnfo) delete g_pPM4Portnfo;
	if (NULL != g_pPM5Portnfo) delete g_pPM5Portnfo;
	if (NULL != g_pPM6Portnfo) delete g_pPM6Portnfo;

	if (NULL != g_pPM1Manager) delete g_pPM1Manager;
	if (NULL != g_pPM2Manager) delete g_pPM2Manager;
	if (NULL != g_pPM3Manager) delete g_pPM3Manager;
	if (NULL != g_pPM4Manager) delete g_pPM4Manager;
	if (NULL != g_pPM5Manager) delete g_pPM5Manager;
	if (NULL != g_pPM6Manager) delete g_pPM6Manager;
}

BOOL InitializePortInfo()
{
	BOOL bRet = TRUE;
	
	FILE *fp;
	char szRead        [256];
	char szItem        [256];
	char szModuleName  [32];
	unsigned short  usPortNum;


	do {
		fp = fopen(PORT_INFO_FILE, "rt");
		if (fp == NULL) {
			printf("file %s open fail \n" , PORT_INFO_FILE);
			bRet = FALSE; break;
		}
		
		if (NULL == fgets(szRead, 255, fp)) {
			printf("file %s fgets fail \n" , PORT_INFO_FILE);
			bRet = FALSE; break;
		}
		
		do {
			if (szRead[0] == '[') {
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szModuleName, ']');
				do {
					if (NULL == fgets(szRead, 255, fp)) break;
					if (szRead[0] == '[') break;
					if (szRead[0] != ';' && szRead[0] != '#') {
						CTextParser tp1(szRead);
						// Read Tag
						if(FALSE == tp1.GetWord(szItem, 0)) {
							bRet = FALSE; break;
						}

						if (0 == strcmp(szItem , "$$USED_PORT")) {
							tp1.GetInt((int*)&usPortNum, 0);
							if (0 == strcmp(szModuleName , "PM1")) {
								if (g_pPM1Portnfo == NULL) {
									g_pPM1Portnfo = new CPortInfo();
									if (false == g_pPM1Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM1Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM1 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else if (0 == strcmp(szModuleName , "PM2")) {
								if (g_pPM2Portnfo == NULL) {
									g_pPM2Portnfo = new CPortInfo();
									if (false == g_pPM2Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM2Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM2 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else if (0 == strcmp(szModuleName , "PM3")) {
								if (g_pPM3Portnfo == NULL) {
									g_pPM3Portnfo = new CPortInfo();
									if (false == g_pPM3Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM3Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM3 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else if (0 == strcmp(szModuleName , "PM4")) {
								if (g_pPM4Portnfo == NULL) {
									g_pPM4Portnfo = new CPortInfo();
									if (false == g_pPM4Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM4Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM4 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else if (0 == strcmp(szModuleName , "PM5")) {
								if (g_pPM5Portnfo == NULL) {
									g_pPM5Portnfo = new CPortInfo();
									if (false == g_pPM5Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM5Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM5 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else if (0 == strcmp(szModuleName , "PM6")) {
								if (g_pPM6Portnfo == NULL) {
									g_pPM6Portnfo = new CPortInfo();
									if (false == g_pPM6Portnfo->SetModuleName(szModuleName)) {
										bRet = FALSE; break;
									}
									if (false == g_pPM6Portnfo->SetPortNum(usPortNum)) {
										bRet = FALSE; break;
									}
									break;
								} else {
									printf ("PM6 Port aleady exist \n");
									bRet = FALSE; break;
								}
							} else {
								bRet = FALSE; break;
							}
						}
					}
				} while (feof(fp) == 0);
			}
		
			if (FALSE == bRet) break;
			if (NULL == fgets(szRead, 255, fp)) //end of file
				break;
		} while(feof(fp) == 0);

		fclose(fp);
	} while(0);

	if (bRet == FALSE) {
		if (NULL != fp) fclose(fp);
		printf("-----> Config File Reading Failed : %s", PORT_INFO_FILE);
	}

	return bRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main()
{

	//Initialize Port Information
	if (FALSE == InitializePortInfo()) {

		DeInitializeMain();

		printf ("Initialize Port Info Fail...!!");
		return SYS_ABORTED;
	}

    g_pWindowInterface = new CWindowInterface();

	//Create Ethernet Manager
	if (NULL != g_pPM1Portnfo) {
		g_pPM1Manager = new CEthernetManager(g_pPM1Portnfo);
		if (false == g_pPM1Manager->Initialize(g_pWindowInterface)) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

	if (NULL != g_pPM2Portnfo) {
		g_pPM2Manager = new CEthernetManager(g_pPM2Portnfo);
		if (false == g_pPM2Manager->Initialize(g_pWindowInterface)) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

	if (NULL != g_pPM3Portnfo) {
		g_pPM3Manager = new CEthernetManager(g_pPM3Portnfo);
		if (false == g_pPM3Manager->Initialize(g_pWindowInterface)) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

	if (NULL != g_pPM4Portnfo) {
		g_pPM4Manager = new CEthernetManager(g_pPM4Portnfo);
		if (false == g_pPM4Manager->Initialize(g_pWindowInterface)) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

	if (NULL != g_pPM5Portnfo) {
		g_pPM5Manager = new CEthernetManager(g_pPM5Portnfo);
		if (false == g_pPM5Manager->Initialize(g_pWindowInterface)) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

	if (NULL != g_pPM6Portnfo) {
		g_pPM6Manager = new CEthernetManager(g_pPM6Portnfo);
		if (false == g_pPM6Manager->Initialize()) {
			DeInitializeMain();
			return SYS_ABORTED;
		}				
	}

    
	////////////////////////////////////////////////////////////////////////////////////
	//Dummy loop
	while (TRUE) {
		if (FALSE == WAIT_SECONDS(1.0)) {
			DeInitializeMain();
			printf ("IO Monitor Function abnormally escape..\n");
			return SYS_ABORTED;
		}	
	}	
	////////////////////////////////////////////////////////////////////////////////////

	DeInitializeMain();
	return SYS_SUCCESS;
}

