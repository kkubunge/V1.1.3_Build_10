// DataProcessor.h: interface for the CDataProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAPROCESSOR_H__50EFFBB1_F64F_4AA1_9643_EED85099A90D__INCLUDED_)
#define AFX_DATAPROCESSOR_H__50EFFBB1_F64F_4AA1_9643_EED85099A90D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Utility.h"

class CWindowInterface;
class CEthernetManager;
class CIOInfo;
class CDataProcessor  
{
public:
	CDataProcessor(CEthernetManager* pEthernetManager = NULL);
	virtual ~CDataProcessor();

	void  SetWindowInterface(CWindowInterface* pWindowInterface = NULL);
	bool  InitIOConfig ();
	bool  InitIOPoints ();
	char* GetModuleName();

	//IO Log
	bool  GetConnectionMessage   ();
	bool  GetDisconnectionMessage();

	bool  GetLogStartMessage(IO_DATA_STRUCT* sIOData);
	bool  GetLogStopMessage (IO_DATA_STRUCT* sIOData);
	bool  GetDNetFailMessage(IO_DATA_STRUCT* sIOData);
	bool  GetNormalIOMessage(IO_DATA_STRUCT* sIOData);

private:
	CWindowInterface* m_pWindowInterface;
	CEthernetManager* m_pEthernetManager;
	CIOInfo*          m_pIOInfo[MAX_IO];
	long              m_enIOCount;
	long              m_enLogCount;

	FILE*             m_fp;
	char              m_szFilePath[256];

	int               m_nIOCM1JobID;
	int               m_nIOCM2JobID;
	int				  m_nIOCM3JobID;
	int               m_nIOPMWaferSrc;
	int               m_nIOPMWaferSts;

};

#endif // !defined(AFX_DATAPROCESSOR_H__50EFFBB1_F64F_4AA1_9643_EED85099A90D__INCLUDED_)
