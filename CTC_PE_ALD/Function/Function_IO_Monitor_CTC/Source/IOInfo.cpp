// IOInfo.cpp: implementation of the CIOInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextParser.h"
#include "IOInfo.h"
#include "DataProcessor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIOInfo::CIOInfo(CDataProcessor* pDataProcessor)
{
	if (NULL != pDataProcessor)
		m_pDataProcessor = pDataProcessor;
	
	memset(m_szIOName , 0 , sizeof(m_szIOName));
	m_bIsLookupNeed = false;

}

CIOInfo::~CIOInfo()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : CreateIO
//Date         : 2007.03.13
//Description  : Get IO Address & Lookup Table Details..
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CIOInfo::CreateIO(long enIOType , char* szAddress)
{
	bool bRet = true;
	char szIOName[32];
	char szDummy [32];
	char szDrvName[32];
	char szLookup[32];

	do {
		CTextParser tp(szAddress);
		if (DIGITAL == enIOType) {
			m_enIOType = DIGITAL;

			//Get IO Name
			tp.GetWord(szIOName , 0);
			if (NULL == szIOName) return false;
			else {
				strcpy(m_szIOName , szIOName);
			}

			//Get Enum but Not using
			tp.GetWord(szDummy);

			//Get Drv but Not using
			tp.GetWord(szDrvName);
			if (NULL == szDrvName || szDrvName[0] == '-') return false;

			//Get 
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-' || szDummy[0] != '1') return false;

			//Get Start Byte
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-') return false;
			if (atoi(szDummy) < 0 || atoi(szDummy) > 400) return false;
			else m_nStartByte = atoi(szDummy);

			//Get Start Bit
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-') return false;
			if (atoi(szDummy) < 0 || atoi(szDummy) > 15) return false;
			else m_nStartBit = atoi(szDummy);

			//Get End Bit
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-') return false;
			if (atoi(szDummy) < 0 || atoi(szDummy) > 15) return false;
			else m_nEndBit = atoi(szDummy);

		} else if (ANALOG == enIOType) {
			m_enIOType = ANALOG;

			//Get IO Name
			tp.GetWord(szIOName , 0);
			if (NULL == szIOName) return false;
			else {
				strcpy(m_szIOName , szIOName);
			}

			//Get Unit but Not using
			tp.GetWord(szDummy);

			//Get Drv but Not using
			tp.GetWord(szDrvName);
			if (NULL == szDrvName || szDrvName[0] == '-') return false;

			//Get 
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-' || szDummy[0] != '1') return false;

			//Get Start Byte ID1
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-') return false;
			if (atoi(szDummy) < 0 || atoi(szDummy) > 400) return false;
			else m_nStartByte = atoi(szDummy);

			//Get ID2
			tp.GetWord(szDummy);
			if (NULL == szDummy || szDummy[0] == '-') return false;
			if (atoi(szDummy) < 0 || atoi(szDummy) > 10) return false;
            else m_nID2 = atoi(szDummy);

			//Get Dummy
			tp.GetWord(szDummy);
			tp.GetWord(szDummy);
			if (szDummy[0] != '0' && szDummy[0] != 'L') return false;

			if (szDummy[0] == '0') {
				//scale?
				tp.GetWord(szDummy);
			} else {
				tp.GetWord(szLookup);
				if (false == CreateLookup(szLookup)) return false;		
				
				m_bIsLookupNeed = true;
			}
		} else return false;
	} while (0);


	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : CreateLookup
//Date         : 2007.03.13
//Description  : Save Lookup Table into struct
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CIOInfo::CreateLookup(char* szLookup)
{
	bool bRet = true;

	FILE *fp;
	char szRead        [256];
	char szItem		   [256];
	char szFile        [256];
	bool bCompleted = false;
	
	do {
		strcpy(szFile , IO_INFO_FILE);
		strcat(szFile , m_pDataProcessor->GetModuleName());
		strcat(szFile , "\\LookUp.io");
		
		fp = fopen(szFile, "rt");
		if (fp == NULL) {
			printf("file %s open fail \n" , szFile);
			bRet = FALSE; break;
		}

		if (NULL == fgets(szRead, 255, fp)) {
			printf("file %s fgets fail \n" , szFile);
			bRet = FALSE; break;
		}

		do {
			CTextParser tp(szRead);
			tp.GetWord(szItem, 0);
			if (szRead[0] == ';' || szRead[0] == '#' || szRead[0] == '\n' || szRead[0] == '\t') {
				//Do Nothing
			} else {
				if (0 == strcmp(szItem , szLookup)) {
					do {
						//Read Lookup Start
						CTextParser tp1(szRead);
						tp1.GetWord(szItem);
						if (szItem[0] != '-' && 0 != strcmp(szItem , szLookup)) {
							bCompleted = true; break;
						} else {
							tp1.GetDbFloat(&m_sLookupTable.dbLogical [m_sLookupTable.nLookupCount]);
							tp1.GetDbFloat(&m_sLookupTable.dbPhysical[m_sLookupTable.nLookupCount]);
							m_sLookupTable.nLookupCount++;
						}

						if (NULL == fgets(szRead, 255, fp))
							break;
					} while(feof(fp) == 0);
				}
			}
			
			if (false == bRet) break;
			if (true  == bCompleted) break;
			if (NULL == fgets(szRead, 255, fp)) //end of file
				break;
			
		} while(feof(fp) == 0);

		fclose(fp);
	} while (0);

	if (bRet == FALSE) {
		if (NULL != fp) fclose(fp);
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : CalCurrentIOValue
//Date         : 2007.03.13
//Description  : Calculate current io value from io memory directly
//Arguments    :
//Return Value : bool
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CIOInfo::CalCurrentIOValue(unsigned char* ucIOMem)
{
	bool bRet = true;

	do {
		WORD  wData  = 0;
		DWORD dwData = 0;
		
		if (DIGITAL == m_enIOType) {
			wData  = *(ucIOMem + m_nStartByte + 1);
			wData  = (wData << 8) + *(ucIOMem + m_nStartByte);
			wData  =  wData << (UPPER_BIT - m_nEndBit);
			wData  =  wData >> (UPPER_BIT - m_nEndBit + m_nStartBit);

			m_nCurrentValue = (int)wData;
		} else if (ANALOG == m_enIOType) {
			dwData = *(ucIOMem + m_nStartByte + 1);
			dwData = (dwData << 8) + *(ucIOMem + m_nStartByte);

			if (5 == m_nID2) {
				signed short sData = (signed short) dwData;
				m_dbCurrentValue = (double)sData;
			} else m_dbCurrentValue = dwData;

			if (true == m_bIsLookupNeed) {
				m_dbCurrentValue = ConversionPhysicalToLogical(m_dbCurrentValue);
			}
		}	
	}while(0);


	return bRet;
}

//------------------------------------------------------------------------------------------------------------------------
//Name        : ConversionPhysicalToLogical
//Author      : 
//Date        : 2007.03.20
//Return      : double
//Description : 
//------------------------------------------------------------------------------------------------------------------------
double  CIOInfo::ConversionPhysicalToLogical(double dbRawData)
{
	double dbReturn         = 0.0f;

	double dbFirstLogical   = 0.0f;
	double dbSecondLogical  = 0.0f;
	double dbFirstPhysical  = 0.0f;
	double dbSecondPhysical = 0.0f;
	double dbSlopeInverse   = 0.0f;


	for (int i = 0 ; i < m_sLookupTable.nLookupCount; i++) {
		dbSecondLogical  = m_sLookupTable.dbLogical[i];
		dbSecondPhysical = m_sLookupTable.dbPhysical[i];
		
		if (dbRawData > dbSecondPhysical) continue;
		if (i == 0) {
			//then the given value is less than that in the Map so return the Minimum value
			dbSlopeInverse = 0;
			break;
		}

		//for getting prefer value to calculation
		dbFirstLogical  = m_sLookupTable.dbLogical[i-1];
		dbFirstPhysical = m_sLookupTable.dbPhysical[i-1];
		dbSlopeInverse  = (double)((double)(dbSecondLogical - dbFirstLogical)/(double)(dbSecondPhysical  - dbFirstPhysical));
		break;
	}

	if (i == m_sLookupTable.nLookupCount) {
		dbReturn = 0.0f;
		return dbReturn;
	}

	// X axis - Voltage and Y axis Pressure
	//X = (Y - Y1) / Slope + X1;
	dbReturn  = (double)((double)(dbRawData - dbSecondPhysical) * (double)dbSlopeInverse) + (double)dbSecondLogical;

	return dbReturn;
}

