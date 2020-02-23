#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <kutlStr.h>
#include <kutlAnal.h>
#include <kutlFile.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

//#define		MAX_USER_COUNT			50	// �ִ� ��� �Ҽ� �ִ� ����ڼ�
#define		MIN_AUTO_LOGOUT_TIME	1	// �ּ� Logout Time(����:��)


#ifndef _DEBUG
  // #define _DEBUG
#endif

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
enum	{ OFF_0	 ,	ON_1 } ;

// AutoLogOut ��� Mode
typedef enum{
	RUN_MODE_NOTUSE,	// Auto Logoff �����������
	RUN_MODE_ALL,		// ��� User Auto LogOff ���
	RUN_MODE_SPECIAL	// ȯ������ ���
}RUN_MODE;

// RUN_MODE_SPECIAL Mode�� ������ ��� �� ����� ������ ���� ����ü
typedef struct {
	char	ITEM[65];
	DWORD	DATA;
}USER_LOGOUT_INFO;
USER_LOGOUT_INFO gUsrLogOutInfos[2]= {0,};

// ITEM ����
DWORD	gnCount = 0;

UINT	gnRunMode;				// AutoLogOut ���� MODE
UINT	gnAutoLogOffTime;		// ���� ����ڰ� ������ AutoLogOutTime(����:min)
BOOL	gbUserNoAction = FALSE;	// ���� ����� �Է� ����[TRUE(�Է���),FALSE(�Է¹�)]

extern __declspec(dllimport) BOOL StartHook();
extern __declspec(dllimport) BOOL IsUserAction(long out_min);

void MAIN_THREAD();
void LoadUserLogOutParam(char *parm_file);
DWORD GetAutoLogOutData(char *ITEM);

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code()
{
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Event_Message_Received()
{
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code()
{
	//char    RUN_STR[256];
	//char	ELSE_STR[256];
	//char	ITEM1[128],ITEM2[128] ;
	int 	CommStatus;

//	printf("< AUTO LOGOUT START >\n");

//	STR_SEPERATE ( PROGRAM_ARGUMENT_READ() , RUN_STR , ELSE_STR , 20 );
	//sprintf( RUN_STR , PROGRAM_ARGUMENT_READ() );
//	printf("------------------------------------------\n");
//	printf("autologout��� Read!!\n\t* ȯ�� ���ϰ��(%s)\n",RUN_STR,ELSE_STR);

	// LoadUserLogOutParam(RUN_STR);

	// sprintf( ITEM1 ,"AutoLogOut_Use" );
	// sprintf( ITEM2 ,"AutoLogOut_Time" );
	// gnRunMode = GetAutoLogOutData(ITEM1);
	// gnAutoLogOffTime = GetAutoLogOutData(ITEM2);

//	printf("------------------------------------------\n");
//	printf("autologout���� Read!!\n\t* AutoLogOut Mode:%d\n\t* AutoLogOut Time:%d\n",gnRunMode,gnAutoLogOffTime ) ;
//	printf("------------------------------------------\n");

	//	if(gnAutoLogOffTime < MIN_AUTO_LOGOUT_TIME)	gnAutoLogOffTime = 1;	// Default LogOutTime 1min

	gnAutoLogOffTime = READ_DIGITAL(PRMA_AutoLogOut_TO, &CommStatus);

	if(gnAutoLogOffTime < MIN_AUTO_LOGOUT_TIME)
	{
		gnRunMode = RUN_MODE_NOTUSE;
	}
	else
	{
		gnRunMode = RUN_MODE_ALL;
	}

	_beginthread( (void *) MAIN_THREAD , 0 , 0 );

	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info(int no)
{
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void MAIN_THREAD()
{
	int CommStatus;
	int usrLevel;
	char usrName[65];
	UINT changeCheckTime;

#ifdef _DEBUG
	if(StartHook())	printf("--->Hook start ����\n");
	else			printf("--->Hook start ����\n");
#else
	StartHook();
#endif

	do
	{
		// 1. ���� ���� ��
		usrLevel = READ_DIGITAL( UserLevelControl , &CommStatus );
		READ_STRING(CurrentUserName, usrName, &CommStatus);

		if(usrLevel == -1 || strlen(usrName)==0)
		{
#ifdef _DEBUG
			printf("--->LogOut ����\n");
#endif
			continue;
		}

		// if(gnAutoLogOffTime <= MIN_AUTO_LOGOUT_TIME) {
		// 	gnAutoLogOffTime = MIN_AUTO_LOGOUT_TIME;
		// }
		changeCheckTime = READ_DIGITAL( PRMA_AutoLogOut_TO , &CommStatus );

		if (changeCheckTime == gnAutoLogOffTime)
		{
			// gnAutoLogOffTime == 0 �̸�, AutoLogOut ����� ������� �ʴ� ������ ����.
			if((gnAutoLogOffTime > 0) && ( !IsUserAction(gnAutoLogOffTime)))
			{
				if( gnRunMode != RUN_MODE_NOTUSE ){
					RUN_FUNCTION(UserControl, "LOGOUT") ;
					printf("--->LogOutTime [%d] \n", gnAutoLogOffTime);
				}
			}
		}
		else
		{
			gnAutoLogOffTime = changeCheckTime;
			if(gnAutoLogOffTime < MIN_AUTO_LOGOUT_TIME)
			{
				gnRunMode = RUN_MODE_NOTUSE;
			}
			else
			{
				gnRunMode = RUN_MODE_ALL;
			}
		}

#ifdef _DEBUG
		printf("--->LogOutTime[%d] NoAct[%d]\n", gnAutoLogOffTime, gbUserNoAction);
#endif

	}while(WAIT_SECONDS(1));

	_endthread();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void LoadUserLogOutParam(char *parm_file)
{
	FILE * fpt;
	char Buffer[256];
	char ITEM [ 128 + 1 ];
	char DATA [ 128 + 1 ];

	fpt= fopen( parm_file, "r");
	if(fpt)
	{
		while(Read_Line_From_File(fpt, Buffer) )
		{
			STR_SEPERATE( Buffer,  ITEM , DATA , 128 );
			if(strlen(ITEM))
			{
				strcpy(gUsrLogOutInfos[gnCount].ITEM, ITEM);
				gUsrLogOutInfos[gnCount].DATA = atoi(DATA);
				gnCount++;
			}
		}
		fclose(fpt);

	}
	else
	{
		// RUN_MODE_SPECIAL mode ������, �ܺ� ȯ�������� ã�� �� ������ RUN_MODE_NOTUSE�� ����
		printf("--->AutoLogOut Argument File(%s) Not Found... AutoLogOut Not Used", parm_file);
		gnRunMode = RUN_MODE_NOTUSE;
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
DWORD GetAutoLogOutData(char *ITEM)
{
	DWORD i;
	DWORD DATA = 0;
	for(i=0; i<2; i++)
	{
		if(STRCMP_L(gUsrLogOutInfos[i].ITEM, ITEM))
		{
			DATA = gUsrLogOutInfos[i].DATA;
			break;
		}
	}

	return DATA;
}