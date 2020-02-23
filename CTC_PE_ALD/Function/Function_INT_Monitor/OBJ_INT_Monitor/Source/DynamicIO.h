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
2005.12.07	Sample에 설명 잘못된 부분 수정(m_pIOList => g_aIOList).
2005.12.20	<cskim> 동적으로 Function을 사용할 수 있도록 하는 기능 추가
			- dRUN_FUNCTION(...)
			- dRUN_FUNCTION_FREE(...)
			- dREAD_FUNCTION(...)
2005.12.20	<cskim(2)> dRUN_FUNCTION(...) Function의 return 값을 갖도록 수정.
			dRUN_WAIT_FUNCTION(...) 추가
2006.03.30	I/O 들을 개별 등록하여 사용할 수 있도록 하는 기능 추가
			- RegistIOChannel(char *szIOName, int nIOType);
			- GetIoIndex(char *szIOName);
			- InitIOChannel(int nIoIndex);
2006.09.28	<ycchoi> dWRITE_FUNCTION(...) 추가
2006.11.22	<cskim> ReplaceIONameChar(...) Func 추가
			IO Name의 특정 문자를 지정 문자로 치완하는 Function
2006.11.24	<cskim> I/O Type 않맞을 경우 Read/Write 수행시 Console에
			Error를 표시하도록 수정.
2006.12.01	<cskim> InitIOList(...) Function에서 IO Valid Check 기능의
			마지막 I/O Index를 9999로 수정. 이 수정 내용은 Default로
			모든 I/O를 Check하도록 하기 위해서임.
2006.12.20	m_nIOCount 를 파생 class에서 참조할 수 있도록 Protected로
			변경
2008.10.23	dREAD_DIGITAL_TRUE Function 추가
			I/O Reading을 실시간 읽기 위한 Function
2008.12.03	dWRITE_DIGITAL_FROM_NAME Func 추가
			dWRITE_ANALOG_FROM_NAME Func 추가 
			dWRITE_IO_FROM_NAME Func 추가
			Channel Name 을 이용하여 I/O를 Write하는 기능
			Engine 에 부하를 줄 수 있으므로 회수가 잦은 반복적인 Routine에는
			사용하지 말것.
2008.12.17	ClearAndInitIOList(...) Func 추가
			I/O List 를 'NotDefined' 로 초기화 한다.
			'NotDefined'로 초기화 된 영역은 RegistIOChannel(..) Func을
			사용하여 NotDefined I/O영역에 I/O를 추가하여 사용할 수 있다.
--------------------------------------------------------------------*/
