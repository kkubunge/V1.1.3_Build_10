// DynamicIO.h: interface for the CDynamicIO class.
// Create : 2005.11.01
// Last Update : 2005.12.20(2)
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNAMICIO_H__69A96A6D_AC54_41AB_ACC0_BE69D32D76FD__INCLUDED_)
#define AFX_DYNAMICIO_H__69A96A6D_AC54_41AB_ACC0_BE69D32D76FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
typedef struct _IO_LIST
{
	char szIOName[40];
	int nIOType;
	int nIOPoint;
} IO_LIST;

//--------------------------------------------------------------------
class CDynamicIO  
{
public:
	CDynamicIO();
	virtual ~CDynamicIO();

	// Member Function
public:
	BOOL InitIOList(IO_LIST pIOList[], bool bIOValidErrCheck = true, int nIOValidCheckIdx = 0);
	BOOL IOValid(int nIoIdxEnum);

	void dWRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult);
	int dREAD_DIGITAL(int nIoIdxEnum, BOOL *pResult);

	void dWRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult);
	double dREAD_ANALOG(int nIoIdxEnum, BOOL *pResult);

	void dWRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);
	char *dREAD_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);

	int dRUN_FUNCTION(int nIoIdxEnum, char *szParam);
	int dRUN_WAIT_FUNCTION(int nIoIdxEnum);
	BOOL dRUN_FUNCTION_FREE(int nIoIdxEnum, char *szParam);
	BOOL dRUN_FUNCTION_ABORT(int nIoIdxEnum);
	BOOL dRUN_SET_FUNCTION(int nIoIdxEnum, char *szParam);
	BOOL dWRITE_FUNCTION_EVENT(int nIoIdxEnum, char *szParam);
	int dREAD_FUNCTION(int nIoIdxEnum);

private:
	bool m_bIOValidErrCheck;
	IO_LIST *m_pIOList;
	int m_nIOCount;
};

#endif // !defined(AFX_DYNAMICIO_H__69A96A6D_AC54_41AB_ACC0_BE69D32D76FD__INCLUDED_)
//////////////////////////////////////////////////////////////////////
/* Sample
//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	PRO_PostRunDM,
	PRO_PostCmptDM,
	PRMD_Refill_Opt,
	NotDefined_1,
	NotDefined_2,
	Valve10;
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"PRO_PostRunDM",		_K_D_IO, 0},
	{"PRO_PostCmptDM",		_K_D_IO, 0},
	{"PRMD_Refill_Opt",		_K_D_IO, 0},
	{"NotDefined_1",		_K_D_IO, 0},
	{"NotDefined_2",		_K_D_IO, 0},
	{"Valve10",				_K_D_IO, 0},
	""
};
//--------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////
/* Update History
2005.11.01	Created.
			�������� I/O�� ���� �� ����� �����ϰ� �ϱ� ���� Class ����.
			C-Code�� �Ǿ� �ִ� Function�� Class�� �籸����.
2005.11.02	InitIOList() Function�� I/O Name�� �������� ���� ���
			Error�� ȭ�鿡 Print�ϴ� Option �߰�
2005.11.16	Debug Mode Compile�� ���� Code������ Channel I/O Error
			��Ȳ�� Console�� Print �ǵ��� ����.
2005.11.19	<cskim> I/O�� �������� �� Console ȭ�鿡 Print�Ǵ� ���� ����.
			InitIOList() Function�� BOOL Type Parameter�� bool Type����
			����.
2005.11.22	InitIOList() ���� I/O Valid Check�ÿ� Check�ؾ��� I/O
			������ ���� ��Ű�� ��� �߰�.
2005.11.25	Init IO Fail �ÿ� Console ȭ�鿡 Print �Ǵ� ���� ����.
2005.12.07	Sample�� ���� �߸��� �κ� ����(m_pIOList => g_pIOList).
2005.12.20	<cskim> �������� Function�� ����� �� �ִ� ��� �߰�
			- dRUN_FUNCTION(...)
			- dRUN_FUNCTION_FREE(...)
			- dREAD_FUNCTION(...)
2005.12.20	<cskim(2)> dRUN_FUNCTION(...) Function�� return ���� ������ ����.
			dRUN_WAIT_FUNCTION(...) �߰�
--------------------------------------------------------------------*/
