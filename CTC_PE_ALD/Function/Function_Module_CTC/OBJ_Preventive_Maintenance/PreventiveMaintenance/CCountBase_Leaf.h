// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCOUNTBASE_LEAF_40C070970251_INCLUDED
#define _INC_CCOUNTBASE_LEAF_40C070970251_INCLUDED

#include "CPreventiveMaintenance_Component.h"

class CCountBase_Leaf 
: public CPreventiveMaintenance_Component
{
public:
	void InitializeChannelInformation(int nCurrentValueChannelID, int nWarningValueChannelID, int nAlarmValueChannelID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost);
	virtual void EnableMonitoring(BOOL bEnable, int nLeafType);
	virtual void ResetCurrentValue();
	virtual int ChangeCurrentValue(void* pData, int nLeafType);
	virtual void RefreshWarningAlarmValue();

	CCountBase_Leaf(int nLeafType , int nCompositeType);

	// added by ycchoi 2006.11.06 < V 1.0.0.6 >
	virtual void RefreshCurrentValue();

	// added by ycchoi 2006.05.28 < V 1.0.0.2>
	virtual int GetCurrentWaferCount();
	virtual int GetType();

	//2008.09.30 Type1 Use this function also
	virtual bool IsPostRunTime(int nType);

	//2009.08.21 by mgsong
	virtual int GetWaringID();
	virtual int GetAlarmID();



protected:
	BOOL m_bAlarmPost;
	int  m_nEnableChannelID;
	int  m_nWarningID;
	int  m_nAlarmID;
	int  m_nCurrentValueChannelID;
	int  m_nWarningValueChannelID;
	int  m_nAlarmValueChannelID;
	int  m_nCurrentValue;
	int  m_nWarningValue;
	int  m_nAlarmValue;
	int  m_nType;
	int  m_nCompositeType;
	BOOL m_bMonitoring;

	
};

#endif /* _INC_CCOUNTBASE_LEAF_40C070970251_INCLUDED */
