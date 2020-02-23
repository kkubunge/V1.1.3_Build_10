// DynamicIO.h: interface for the CDynamicIO class.
// Create : 2005.11.01
// Last Update : 2008.12.17
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
	void ReplaceIONameChar(IO_LIST pIOList[], char cOrg, char cNew);		// 2006.11.22
	BOOL InitIOList(IO_LIST pIOList[], bool bIOValidErrCheck = true, int nIOValidCheckEndIdx = 9999);	// 2006.12.01
	BOOL IOValid(int nIoIndex);
	void ClearAndInitIOList(IO_LIST pIOList[], int nCount);			// 2008.12.17

	void dWRITE_DIGITAL(int nIoIndex, int nValue, BOOL *pResult);
	int dREAD_DIGITAL(int nIoIndex, BOOL *pResult);
	int dREAD_DIGITAL_TRUE(int nIoIndex, BOOL *pResult);		// 2008.10.22

	void dWRITE_ANALOG(int nIoIndex, double dbValue, BOOL *pResult);
	double dREAD_ANALOG(int nIoIndex, BOOL *pResult);

	void dWRITE_STRING(int nIoIndex, char *pValue, BOOL *pResult);
	char *dREAD_STRING(int nIoIndex, char *pValue, BOOL *pResult);

	//-----------------------------------------------------------------
	// 2008.12.03
	void dWRITE_DIGITAL_FROM_NAME(char *szIOName, int nValue, BOOL *pResult);
	void dWRITE_ANALOG_FROM_NAME(char *szIOName, double dbValue, BOOL *pResult);
	void dWRITE_IO_FROM_NAME(char *szIOName, int nIOType, int nValue, double dbValue, BOOL *pResult);

	int dRUN_FUNCTION(int nIoIndex, char *szParam);
	int dRUN_WAIT_FUNCTION(int nIoIndex);
	BOOL dRUN_FUNCTION_FREE(int nIoIndex, char *szParam);
	BOOL dRUN_FUNCTION_ABORT(int nIoIndex);
	BOOL dRUN_SET_FUNCTION(int nIoIndex, char *szParam);
	BOOL dWRITE_FUNCTION_EVENT(int nIoIndex, char *szParam);
	int dREAD_FUNCTION(int nIoIndex);

	// __BEGIN__ 2006.09.28 by ycchoi
	int dWRITE_FUNCTION(int nIoIdxEnum, char *szParam);
	// __END__ 2006.09.28 by ycchoi

	//-----------------------------------------------------------------
	// 2006.03.30
	int RegistIOChannel(const char *szIOName, int nIOType);
	int GetIoIndex(char *szIOName);


private:
	BOOL InitIOChannel(int nIoIndex);
	//-----------------------------------------------------------------

protected:		// 2006.12.20
	int m_nIOCount;

private:
	bool m_bIOValidErrCheck;
	IO_LIST *m_pIOList;
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
2005.12.07	Sample�� ���� �߸��� �κ� ����(m_pIOList => g_aIOList).
2005.12.20	<cskim> �������� Function�� ����� �� �ֵ��� �ϴ� ��� �߰�
			- dRUN_FUNCTION(...)
			- dRUN_FUNCTION_FREE(...)
			- dREAD_FUNCTION(...)
2005.12.20	<cskim(2)> dRUN_FUNCTION(...) Function�� return ���� ������ ����.
			dRUN_WAIT_FUNCTION(...) �߰�
2006.03.30	I/O ���� ���� ����Ͽ� ����� �� �ֵ��� �ϴ� ��� �߰�
			- RegistIOChannel(char *szIOName, int nIOType);
			- GetIoIndex(char *szIOName);
			- InitIOChannel(int nIoIndex);
2006.09.28	<ycchoi> dWRITE_FUNCTION(...) �߰�
2006.11.22	<cskim> ReplaceIONameChar(...) Func �߰�
			IO Name�� Ư�� ���ڸ� ���� ���ڷ� ġ���ϴ� Function
2006.11.24	<cskim> I/O Type �ʸ��� ��� Read/Write ����� Console��
			Error�� ǥ���ϵ��� ����.
2006.12.01	<cskim> InitIOList(...) Function���� IO Valid Check �����
			������ I/O Index�� 9999�� ����. �� ���� ������ Default��
			��� I/O�� Check�ϵ��� �ϱ� ���ؼ���.
2006.12.20	m_nIOCount �� �Ļ� class���� ������ �� �ֵ��� Protected��
			����
2008.10.23	dREAD_DIGITAL_TRUE Function �߰�
			I/O Reading�� �ǽð� �б� ���� Function
2008.12.03	dWRITE_DIGITAL_FROM_NAME Func �߰�
			dWRITE_ANALOG_FROM_NAME Func �߰� 
			dWRITE_IO_FROM_NAME Func �߰�
			Channel Name �� �̿��Ͽ� I/O�� Write�ϴ� ���
			Engine �� ���ϸ� �� �� �����Ƿ� ȸ���� ���� �ݺ����� Routine����
			������� ����.
2008.12.17	ClearAndInitIOList(...) Func �߰�
			I/O List �� 'NotDefined' �� �ʱ�ȭ �Ѵ�.
			'NotDefined'�� �ʱ�ȭ �� ������ RegistIOChannel(..) Func��
			����Ͽ� NotDefined I/O������ I/O�� �߰��Ͽ� ����� �� �ִ�.
--------------------------------------------------------------------*/
