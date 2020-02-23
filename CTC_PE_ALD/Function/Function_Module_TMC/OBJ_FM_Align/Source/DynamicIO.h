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
			유동적인 I/O를 정의 및 사용을 용이하게 하기 위한 Class 구성.
			C-Code로 되어 있는 Function을 Class로 재구성함.
2005.11.02	InitIOList() Function에 I/O Name이 존재하지 않을 경우
			Error를 화면에 Print하는 Option 추가
2005.11.16	Debug Mode Compile된 실행 Code에서는 Channel I/O Error
			상황에 Console에 Print 되도록 수정.
2005.11.19	<cskim> I/O에 실패했을 때 Console 화면에 Print되는 내용 수정.
			InitIOList() Function의 BOOL Type Parameter를 bool Type으로
			변경.
2005.11.22	InitIOList() 에서 I/O Valid Check시에 Check해야할 I/O
			영역을 한정 시키는 기능 추가.
2005.11.25	Init IO Fail 시에 Console 화면에 Print 되는 내용 수정.
2005.12.07	Sample에 설명 잘못된 부분 수정(m_pIOList => g_pIOList).
2005.12.20	<cskim> 동적으로 Function을 사용할 수 있는 기능 추가
			- dRUN_FUNCTION(...)
			- dRUN_FUNCTION_FREE(...)
			- dREAD_FUNCTION(...)
2005.12.20	<cskim(2)> dRUN_FUNCTION(...) Function의 return 값을 갖도록 수정.
			dRUN_WAIT_FUNCTION(...) 추가
--------------------------------------------------------------------*/
