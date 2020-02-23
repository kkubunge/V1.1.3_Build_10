// Copyright (C) IPS Ltd.

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCOUNTANALOGBASE_LEAF_41105EA4009C_INCLUDED
#define _INC_CCOUNTANALOGBASE_LEAF_41105EA4009C_INCLUDED

#include "CPreventiveMaintenance_Component.h"

class CCountAnalogBase_Leaf 
: public CPreventiveMaintenance_Component
{
public:
	void InitializeChannelInformation(int nCurrentValueChannelID, int nWarningValueChannelID, int nAlarmValueChannelID, int nWarningID, int nAlarmID, int nEnableChannelID, int nAlarmChannelID, BOOL bAlarmPost);
	virtual void EnableMonitoring(BOOL bEnable, int nLeafType);
	virtual void ResetCurrentValue();
	virtual int ChangeCurrentValue(void* pData, int nLeafType);
	virtual void RefreshWarningAlarmValue();

	CCountAnalogBase_Leaf(int nLeafType , int nCompositeType);

	// added by ycchoi 2006.11.06 < V 1.0.0.6 >
	virtual void RefreshCurrentValue();

	virtual bool IsPostRunTime(int nType);		//... 2014.12.22 Add Increase Value for Thickness Period DryClean

protected:
	BOOL   m_bAlarmPost;
	int    m_nEnableChannelID;
	int    m_nWarningID;
	int    m_nAlarmID;
	int    m_nCurrentValueChannelID;
	int    m_nWarningValueChannelID;
	int    m_nAlarmValueChannelID;
	double m_doubCurrentValue;
	double m_doubWarningValue;
	double m_doubAlarmValue;
	int    m_nType;
	int    m_nCompositeType;
	BOOL   m_bMonitoring;	
	double m_doubIncreaseValue;		//... 2014.12.22 Add Increase Value for Thickness Period DryClean
};

#endif /* _INC_CCOUNTANALOGBASE_LEAF_41105EA4009C_INCLUDED */
