// PLCSimulatorType2.h: interface for the CPLCSimulatorType2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLCSimulatorType2_H__C8AE496B_BAAE_4A72_A266_761EEB0D4109__INCLUDED_)
#define AFX_PLCSimulatorType2_H__C8AE496B_BAAE_4A72_A266_761EEB0D4109__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PLCSimulator.h"

class CPLCSimulatorType2  : public CPLCSimulator
{
public:
	CPLCSimulatorType2();
	virtual ~CPLCSimulatorType2();

	// Member Function
protected:
	virtual BOOL ExcuteVRCPLine(unsigned char *pData);
	virtual void SetMFC();
	virtual Module_Status RunValveRecipe();
	virtual void SetRF();		// 2006.03.06
	virtual void SetRF2();
	virtual void SetHTE();
};

#endif // !defined(AFX_PLCSimulatorType2_H__C8AE496B_BAAE_4A72_A266_761EEB0D4109__INCLUDED_)
