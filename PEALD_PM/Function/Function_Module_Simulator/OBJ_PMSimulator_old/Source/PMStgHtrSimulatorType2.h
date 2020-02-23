// StgHtrSimulatorType2.h: interface for the CPMStgHtrSimulatorType2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STGHTRSIMULATORTYPE2_H__ACC7DEA5_8F32_4203_9BFA_7D19ABEEC3ED__INCLUDED_)
#define AFX_STGHTRSIMULATORTYPE2_H__ACC7DEA5_8F32_4203_9BFA_7D19ABEEC3ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PMStgHtrSimulator.h"

class CPMStgHtrSimulatorType2  : public CPMStgHtrSimulator
{
public:
	CPMStgHtrSimulatorType2();
	virtual ~CPMStgHtrSimulatorType2();

// Member Function
public:
	virtual BOOL Initialize();
};

#endif // !defined(AFX_STGHTRSIMULATORTYPE2_H__ACC7DEA5_8F32_4203_9BFA_7D19ABEEC3ED__INCLUDED_)
