// DummySlotInfo.cpp: implementation of the CDummySlotInfo class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DummySlotInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummySlotInfo::CDummySlotInfo()
{

}

CDummySlotInfo::~CDummySlotInfo()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : InitializeMember()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummySlotInfo::InitializeMember()
{
	for (int i = 0 ; i < MAX_SLOT ; i++) {
		m_SDummyInfo[i].m_enAlarmStatus = NO_ALARM;
		m_SDummyInfo[i].m_enOrgMode     = NA;
		m_SDummyInfo[i].m_bChanged      = FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetAlarmStatus()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummySlotInfo::SetAlarmStatus(int nSlotNum , long enSts)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return;
	}

	m_SDummyInfo[nSlotNum].m_enAlarmStatus = enSts;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetAlarmStatus()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : long
//////////////////////////////////////////////////////////////////////////////////////////////////////
long CDummySlotInfo::GetAlarmStatus(int  nSlotNum)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return -1;
	}

	return m_SDummyInfo[nSlotNum].m_enAlarmStatus;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetDummyMode()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummySlotInfo::SetDummyMode(int nSlotNum , long enMode)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return;
	}

	m_SDummyInfo[nSlotNum].m_enOrgMode = enMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetDummyMode()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : long
//////////////////////////////////////////////////////////////////////////////////////////////////////
long CDummySlotInfo::GetDummyMode(int nSlotNum)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return -1;
	}

	return m_SDummyInfo[nSlotNum].m_enOrgMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetChangeFlag()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDummySlotInfo::GetChangeFlag(int nSlotNum)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return FALSE;
	}

	return m_SDummyInfo[nSlotNum].m_bChanged;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : SetChangeFlag()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummySlotInfo::SetChangeFlag(int nSlotNum)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return;
	}

	m_SDummyInfo[nSlotNum].m_bChanged = TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : ResetChangeFlag()
//Date         : 2007.05.08
//Description  : 
//Arguments    :
//Return Value : void
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CDummySlotInfo::ResetChangeFlag(int nSlotNum)
{
	if (nSlotNum < 0 || nSlotNum > MAX_SLOT) {
		printf ("Given Slot Nember %d is not valid \n" , nSlotNum);
		return;
	}

	m_SDummyInfo[nSlotNum].m_bChanged = FALSE;

}