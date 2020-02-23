// InterlockMonitor.h: interface for the CInterlockMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_InterlockMonitor_H__76E3C011_DF30_44C4_BF84_CFC6726E8DCF__INCLUDED_)
#define AFX_InterlockMonitor_H__76E3C011_DF30_44C4_BF84_CFC6726E8DCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IOVariable.h"
#include "MonThread.h"

class CInterlockMonitor  : public CMonThread
{
public:
	CInterlockMonitor();
	virtual ~CInterlockMonitor();

// Member Function
public:
	void Enter_Code();
	virtual BOOL Initialize();
	virtual void Deinitialize();
	BOOL ReadConfiguration();
	BOOL ParseSingInterlockList(char *szData);
	BOOL ParseInterlockTypeList(char *szData);

	Module_Status Sim_Main();		// for Simulation
	Module_Status Real_Main();
	void LOG(const char *szLog);

protected:
	virtual void DoMonitor();
	virtual void DoMonitor(int nThreadNo);

protected:
	void DoSingleInterlockMonitor(int nThreadNo, int nIntIndex);
	virtual void ExecInterlock(int nIntType);

	BOOL ValidateInterlock(int nIntIndex, double *pValue1 = NULL, double *pValue2 = NULL);
	BOOL ValidateDec(int nValue1, int nValue2, int nLogic);
	BOOL ValidateDbl(double dbValue1, double dbValue2, int nLogic);

private:

// Member Variable
protected:
	CIOVariable m_IOVar;

	#define MAX_SINGLE_INTERLOCK	100 // Must be less than [MAX_THREAD * 10]
	#define MAX_IO_CONTROL			10
	#define MAX_INTERLOCK_TYPE		10

	typedef struct _SINGLE_INT_LIST
	{
		int nVarIdx1;
		int nVarIdx2;
		int nLogic;
		int nIOType;

		int nTimeout;
		int nAlarmID;
		int nInterlockType;
	} SINGLE_INT_LIST;
	typedef struct _IO_CONTROL_LIST
	{
		char aIONameList[MAX_IO_CONTROL][40];
		int aIOType[MAX_IO_CONTROL];

		char aIONameList2[MAX_IO_CONTROL][40];
		int aIOType2[MAX_IO_CONTROL];

		int anValue[MAX_IO_CONTROL];
		double adbValue[MAX_IO_CONTROL];
		int nCount;
		int nIntrCtrl;
	} IO_CONTROL_LIST;
	// Interlock Ctrl Method
	enum { INTCT_NO, INTCT_ABORT_LOT };

	int m_nSgIntCount;
	CDynamicIO m_SgIntIO;
	SINGLE_INT_LIST		m_SgIntList[MAX_SINGLE_INTERLOCK];
	IO_CONTROL_LIST		m_IoCtrlList[MAX_INTERLOCK_TYPE];
};

#endif // !defined(AFX_InterlockMonitor_H__76E3C011_DF30_44C4_BF84_CFC6726E8DCF__INCLUDED_)
