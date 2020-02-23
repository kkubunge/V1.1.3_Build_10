// Area.cpp: implementation of the CArea class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Area.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : 
//Description  : CArea Class
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CArea::CArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
{
	memset(m_szAreaName , 0 , sizeof(m_szAreaName));
	strcpy(m_szAreaName , szAreaName);

	m_nSlotCount = nSlotCount;
	m_nCapacity  = nCapacity;
	m_nSWEnum    = nSWEnum;
	m_nIOPoint   = nIOPoint;
	m_bDummyMode = bDummyMode;

	for (int i = 0 ; i < MAX_BM_SLOT; i++)
		m_nBMMode[i] = _INOUT;	
}

CArea::~CArea()
{

}

BOOL CArea::GetDummyMode()
{
	return m_bDummyMode;
}

void CArea::SetBMMode(int& nSlot , int& nMode)
{
	if (BUFFER_TYPE == m_enAreaType) {
		m_nBMMode[nSlot] = nMode;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsTypeOfArea()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : AREA_TYPE
//////////////////////////////////////////////////////////////////////////////////////////////////////
AREA_TYPE CArea::IsTypeOfArea()
{
	return m_enAreaType;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetAreaName()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : char*
//////////////////////////////////////////////////////////////////////////////////////////////////////
char* CArea::GetAreaName()
{
	return m_szAreaName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetSWEnum()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int CArea::GetSWEnum()
{
	return m_nSWEnum;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetMaxWafer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int  CArea::GetMaxWafer()
{
	int nMaxWafer = 0;
	nMaxWafer = m_nSlotCount * m_nCapacity;
	return nMaxWafer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsReadyToSend()
//Date         : 2007.06.25
//Description  : Ready Check by user given information
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CArea::IsReadyToSend(FingerACTType enFingerType , int nSlot1 , int nSlot2)
{
	BOOL bRet = TRUE;
	int  nIOStatus;
	int  i,j ,k;
	BOOL bWaferExist = FALSE;

	if (TRANSFER_TYPE == m_enAreaType) {
		switch (enFingerType) {
		case SLOT1:
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			if ((0 == strcmp("TM" , m_szAreaName)) && (2/*ARM B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)))
				return FALSE;
			//2008.11.30

			if (0 >= READ_DIGITAL(m_nIOPoint , &nIOStatus)) {
				//2014.05.19
				if (1 == m_nCapacity)
					return FALSE;
			}
			else {
				bWaferExist = TRUE;
			}

			if (2 == m_nCapacity /*&& TRUE == GetDummyMode()*/) { //dummy를 안쓰면 한장 처리도 가능
				if (0 >= READ_DIGITAL(m_nIOPoint + 2 , &nIOStatus)) {
					if (FALSE == bWaferExist) {
						//둘다 없다
						return FALSE;
					}
				}
			}
			break;
		case SLOT2:
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			if ((0 == strcmp("TM" , m_szAreaName)) && (1/*ARM A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)))
				return FALSE;
			//2008.11.30

			//2008.01.30 (vacuum B Arm)
			if (0 >= READ_DIGITAL(m_nIOPoint + (2 * m_nCapacity) , &nIOStatus)) {
				//2014.05.19
				if (1 == m_nCapacity)
					return FALSE;
			}
			else {
				bWaferExist = TRUE;
			}

			if (2 == m_nCapacity /* && TRUE == GetDummyMode()*/) {
				if (0 >= READ_DIGITAL(m_nIOPoint + (2 * m_nCapacity) + 2 , &nIOStatus)) {
					if (FALSE == bWaferExist) {
						//둘다 없다
						return FALSE;
					}
				}
			}
			break;
		case SLOT3:
			return FALSE;
			break;
		case SLOT4:
			return FALSE;
			break;
		case ALL: {
			k = 0;
			for (i=0;i<m_nSlotCount;i++) {
				for (j=0;j<m_nCapacity;j++) {
					if (0 >= READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) return FALSE;
					k++;
				}
			}
			break;
		}
		case AUTO: {
			k = 0;
			for (i=0;i<m_nSlotCount;i++) {
				if (0 < READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
					if (1 == m_nCapacity || FALSE == GetDummyMode()) return TRUE;
				} else {bRet = FALSE;}

				k++;

				if (FALSE == bRet) {k++; bRet = TRUE; continue;}
				if (0 < READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {return TRUE;} 
				else return FALSE;
				k++;
			}
			break;
		}
		default: bRet = FALSE;break;
		}
	} else if (CASSETTE_TYPE == m_enAreaType) {
		if (0 != strcmp(m_szAreaName , "BM3")) {
			//1 : Absent , 2 : Present
			if (0 != nSlot1 || 0!= nSlot2) {
				if (0 != nSlot1)
					if (2 != READ_DIGITAL(m_nIOPoint + nSlot1-1  , &nIOStatus)) return FALSE;
				if (0 != nSlot2)
					if (2 != READ_DIGITAL(m_nIOPoint + nSlot2-1  , &nIOStatus)) return FALSE;
			} else return FALSE;
		} else {
			//
			if (0 != nSlot1 || 0!= nSlot2) {
			if (0 != nSlot1)
				if (0 >= READ_DIGITAL(m_nIOPoint + nSlot1-1  , &nIOStatus)) return FALSE;
			if (0 != nSlot2)
				if (0 >= READ_DIGITAL(m_nIOPoint + nSlot2-1  , &nIOStatus)) return FALSE;
			} else return FALSE;
		}
	} else {
		if (0 != nSlot1 || 0!= nSlot2) {
			if (0 != nSlot1)
				if (0 >= READ_DIGITAL(m_nIOPoint + (nSlot1-1)*2  , &nIOStatus)) return FALSE;
			if (0 != nSlot2)
				if (0 >= READ_DIGITAL(m_nIOPoint + (nSlot2-1)*2  , &nIOStatus)) return FALSE;
		} else return FALSE;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : IsReadyToReceive()
//Date         : 2007.06.25
//Description  : Ready Check by user given information
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CArea::IsReadyToReceive(FingerACTType enFingerType , int nSlot1 , int nSlot2)
{
	BOOL bRet = TRUE;
	int  nIOStatus;
	int  i,j ,k;
	BOOL bWaferExist = FALSE;

	if (TRANSFER_TYPE == m_enAreaType) {
		switch (enFingerType) {
		case SLOT1:
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			if ((0 == strcmp("TM" , m_szAreaName)) && (2/*ARM B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)))
				return FALSE;
			//2008.11.30

			//2014.05.19 VTM Arm은 2장을 모두 들고 있는 경우가 아니면 Pick 가능토록
			if (0 < READ_DIGITAL(m_nIOPoint , &nIOStatus)) {
				if (1 == m_nCapacity)
					return FALSE;
				else
					bWaferExist = TRUE;
			}
			
			if (2 == m_nCapacity) {
				if (0 < READ_DIGITAL(m_nIOPoint + 2 , &nIOStatus)) {
					if (TRUE == bWaferExist) {
						//모두 들고 있다 
						return FALSE;
					}
				}
			}
			break;
		case SLOT2:
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only
			if ((0 == strcmp("TM" , m_szAreaName)) && (1/*ARM A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)))
				return FALSE;
			//2008.11.30

			//2008.01.30 (Vacuum B Arm)
			//2014.05.19 VTM Arm은 2장을 모두 들고 있는 경우가 아니면 Pick 가능토록			
			if (0 < READ_DIGITAL(m_nIOPoint + (2 * m_nCapacity) , &nIOStatus)) {
				if (1 == m_nCapacity)
					return FALSE;
				else
					bWaferExist = TRUE;
			}

			if (2 == m_nCapacity) {
				if (0 < READ_DIGITAL(m_nIOPoint + (2 * m_nCapacity) + 2 , &nIOStatus)) {
					if (TRUE == bWaferExist) {
						//모두 들고 있다 
						return FALSE;
					}
				}
			}
			break;
		case SLOT3:
			return FALSE;
			break;
		case SLOT4:
			return FALSE;
			break;
		case ALL: {
			k = 0;
			for (i=0;i<m_nSlotCount;i++) {
				for (j=0;j<m_nCapacity;j++) {
					if (0 < READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) return FALSE;
					k++;
				}
			}
			break;
		}
		case AUTO: {
			k = 0;
			for (i=0;i<m_nSlotCount;i++) {
				if (0 >= READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
					if (1 == m_nCapacity) return TRUE;
				} else {bRet = FALSE;}

				k++;

				if (FALSE == bRet) {k++; bRet = TRUE; continue;}
				if (0 >= READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {return TRUE;}
				else return FALSE;
				k++;
			}
			break;
		}
		default: bRet = FALSE;break;
		}
	} else if (CASSETTE_TYPE == m_enAreaType) {
		//2007.12.06 BM3 move to dummy stacker so changed check method
		if (0 != strcmp(m_szAreaName , "BM3")) {
			//1 : Absent , 2 : Present
			if (0 != nSlot1 || 0!= nSlot2) {
				if (0 != nSlot1)
					if (1 != READ_DIGITAL(m_nIOPoint + nSlot1-1  , &nIOStatus)) return FALSE;
				if (0 != nSlot2)
					if (1 != READ_DIGITAL(m_nIOPoint + nSlot2-1  , &nIOStatus)) return FALSE;
			} else return FALSE;
		} else {
			//
			//2007.07.30 bug fix : BM3 handled as like CM so no need 2 IO just 1 IO per slot
			//                     so change (nSlot1(2)-1)*2 --> nSlot1(2) - 1
			if (0 != nSlot1 || 0!= nSlot2) {
			if (0 != nSlot1)
				if (0 < READ_DIGITAL(m_nIOPoint + nSlot1-1  , &nIOStatus)) return FALSE;
			if (0 != nSlot2)
				if (0 < READ_DIGITAL(m_nIOPoint + nSlot2-1  , &nIOStatus)) return FALSE;
			} else return FALSE;
		}
	} else {
		printf("mgsong Trace : Pass0 Slot %d : %d !! \n" , nSlot1 , nSlot2);
		if (0 != nSlot1 || 0 != nSlot2) {
			if (0 != nSlot1)
				if (0 < READ_DIGITAL(m_nIOPoint + (nSlot1-1)*2  , &nIOStatus)) return FALSE;

			printf("mgsong Trace : Pass !! \n");

			if (0 != nSlot2)
				if (0 < READ_DIGITAL(m_nIOPoint + (nSlot2-1)*2  , &nIOStatus)) return FALSE;

			printf("mgsong Trace : Pass2 !! \n");
		} else return FALSE;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetValidReceive()
//Date         : 2007.06.25
//Description  : Get Valid Arm Slot or Station Slot by half information
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CArea::GetValidReceive(char* TrAreaName , FingerACTType& enFingerType , int& nSlot1 , int& nSlot2)
{
	BOOL bRet = FALSE;
	int  nIOStatus;

	if (TRANSFER_TYPE == m_enAreaType) {
		if (0 == strcmp("FM" , m_szAreaName)) {
			if (TRUE == IsReadyToReceive(ALL , 0 , 0)) {enFingerType = ALL;   return TRUE;}
			else return FALSE;
		} else if (0 == strcmp("TM" , m_szAreaName) || 0 == strcmp("TM2" , m_szAreaName)) {
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only so modified
			if (0/*Using All Arm*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToReceive(AUTO , 0 , 0))  {enFingerType = AUTO; return TRUE;}
				else return FALSE;
			} else if (1/*Arm A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToReceive(SLOT1 , 0 , 0)) {enFingerType = SLOT1; return TRUE;}
				else return FALSE;
			} else if (2/*Arm B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToReceive(SLOT2 , 0 , 0)) {enFingerType = SLOT2; return TRUE;}
				else return FALSE;
			} else return FALSE;
		}	
	} else if (CASSETTE_TYPE == m_enAreaType) {
		//Cassette Type not use this method 
		//Cassette Slot must be checked Source & Wafer Status not automatically
	} else if (BUFFER_TYPE == m_enAreaType) {
		//Check associated transfer style
		if (0 == strcmp("FM" , TrAreaName)) {
			//FM --> BM //OUT Direction
			for (int i = 0 ; i < m_nSlotCount ; i++) {
				if (_IN == m_nBMMode[i]) continue;
				if (1 == m_nCapacity) {
					if (TRUE == IsReadyToReceive(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
				} else if(2 == m_nCapacity) {
					if (TRUE == IsReadyToReceive(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
				} else return FALSE;				
			}
		} else if(0 == strcmp("TM" , TrAreaName) || 0 == strcmp("TM2" , TrAreaName)) {
			//TM --> BM //IN Direction
			for (int i = 0 ; i < m_nSlotCount ; i++) {
				if (_OUT == m_nBMMode[i]) continue;
				if (1 == m_nCapacity) {
					if (TRUE == IsReadyToReceive(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
				} else if(2 == m_nCapacity) {
					if (TRUE == IsReadyToReceive(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
				} else return FALSE;				
			}
		} else return FALSE;
	} else {
		for (int i = 0 ; i < m_nSlotCount ; i++) {
			if (1 == m_nCapacity) {
				if (TRUE == IsReadyToReceive(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
			} else if(2 == m_nCapacity) {
				if (TRUE == IsReadyToReceive(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
			} else return FALSE;
		}
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetValidSend()
//Date         : 2007.06.25
//Description  : Get Valid Arm Slot or Station Slot by half information
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CArea::GetValidSend(char* TrAreaName , FingerACTType& enFingerType , int& nSlot1 , int& nSlot2)
{
	BOOL bRet = FALSE;
	int  nIOStatus;

	if (TRANSFER_TYPE == m_enAreaType) {
		if (0 == strcmp("FM" , m_szAreaName)) {
			if (TRUE == IsReadyToSend(ALL , 0 , 0)) {enFingerType = ALL;   return TRUE;}
			else return FALSE;
		} else if (0 == strcmp("TM" , m_szAreaName) || 0 == strcmp("TM2" , m_szAreaName)) {
			//2008.11.30 VTM Arm Forced Selection IO Added Bluetain Only so modified
			if (0/*Using All Arm*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToSend(AUTO , 0 , 0))  {enFingerType = AUTO; return TRUE;}
				else return FALSE;
			} else if (1/*Arm A Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToSend(SLOT1 , 0 , 0)) {enFingerType = SLOT1; return TRUE;}
				else return FALSE;
			} else if (2/*Arm B Only*/ == READ_DIGITAL(VTMARM_USING_OPTION , &nIOStatus)) {
				if (TRUE == IsReadyToSend(SLOT2 , 0 , 0)) {enFingerType = SLOT2; return TRUE;}
				else return FALSE;
			} else return FALSE;
		}	
	} else if (CASSETTE_TYPE == m_enAreaType) {
		//Cassette Type not use this method 
		//Cassette Slot must be checked Source & Wafer Status not automatically
		return FALSE;
	} else if (BUFFER_TYPE == m_enAreaType) {
		//Check associated transfer style
		if (0 == strcmp("FM" , TrAreaName)) {
			//FM --> BM
			for (int i = 0 ; i < m_nSlotCount ; i++) {
				if (1 == m_nCapacity) {
					if (TRUE == IsReadyToSend(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
				} else if(2 == m_nCapacity) {
					if (TRUE == IsReadyToSend(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
				} else return FALSE;				
			}
		} else if(0 == strcmp("TM" , TrAreaName) || 0 == strcmp("TM2" , TrAreaName)) {
			//TM --> BM
			for (int i = 0 ; i < m_nSlotCount ; i++) {
				if (1 == m_nCapacity) {
					if (TRUE == IsReadyToSend(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
				} else if(2 == m_nCapacity) {
					if (TRUE == IsReadyToSend(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
				} else return FALSE;				
			}
		} else return FALSE;
	} else {
		for (int i = 0 ; i < m_nSlotCount ; i++) {
			if (1 == m_nCapacity) {
				if (TRUE == IsReadyToSend(AUTO , i+1 , 0))       {nSlot1 = i+1; nSlot2 = 0;       return TRUE;}
			} else if(2 == m_nCapacity) {
				if (TRUE == IsReadyToSend(AUTO , i*2+1 , i*2+2)) {nSlot1 = i*2+1; nSlot2 = i*2+2; return TRUE;}
			} else return FALSE;			
		}
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetCurWaferInfo()
//Date         : 2007.06.25
//Description  : Get Full Information because user never knows any current information
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL  CArea::GetCurWaferInfo(FingerACTType& enFingerType , int& nDesStation1 , int& nDesStation2 , int& nDesSlot1 , int& nDesSlot2 , BOOL& bStepMode)
{
	BOOL bRet = FALSE;
	int  nIOStatus;
	
	if (TRANSFER_TYPE == m_enAreaType) {
		if (0 == strcmp("FM" , m_szAreaName)) {
			if      (TRUE == IsReadyToSend(ALL   , 0 , 0)) {enFingerType = ALL;}
			else if (TRUE == IsReadyToSend(SLOT1 , 0 , 0)) {enFingerType = SLOT1;}
			else if (TRUE == IsReadyToSend(SLOT2 , 0 , 0)) {enFingerType = SLOT2;}
			else return FALSE;

			if (ALL == enFingerType) {
				nDesSlot1    = READ_DIGITAL(m_nIOPoint   , &nIOStatus); 
				nDesSlot2    = READ_DIGITAL(m_nIOPoint+2 , &nIOStatus); 
				nDesStation1 = READ_DIGITAL(m_nIOPoint+1 , &nIOStatus); 
				nDesStation2 = READ_DIGITAL(m_nIOPoint+3 , &nIOStatus);

				if (nDesStation1 != nDesStation2) bStepMode = TRUE;
				return TRUE;
			} else if (SLOT1 == enFingerType) {
				nDesSlot1    = READ_DIGITAL(m_nIOPoint   , &nIOStatus); 
				nDesSlot2    = 0;
				nDesStation1 = READ_DIGITAL(m_nIOPoint+1 , &nIOStatus); 
				nDesStation2 = 0;
				return TRUE;
			} else if (SLOT2 == enFingerType) {
				nDesSlot1    = 0; 
				nDesSlot2    = READ_DIGITAL(m_nIOPoint+2 , &nIOStatus); 
				nDesStation1 = 0; 
				nDesStation2 = READ_DIGITAL(m_nIOPoint+3 , &nIOStatus);
				return TRUE;
			}
		} else if (0 == strcmp("TM" , m_szAreaName) || 0 == strcmp("TM2" , m_szAreaName)) {
			if (TRUE == IsReadyToSend(SLOT1 , 0 , 0)) {enFingerType = SLOT1;}
			else {
				if (m_nSlotCount == 2) {
					if (TRUE == IsReadyToSend(SLOT2 , 0 , 0)) {enFingerType = SLOT2;}
					else return FALSE;
				} else return FALSE;
			}	
			
			if (SLOT1 == enFingerType) {
				nDesSlot1    = READ_DIGITAL(m_nIOPoint    , &nIOStatus);
				nDesStation1 = READ_DIGITAL(m_nIOPoint+1 , &nIOStatus); 				
				if (2 == m_nCapacity) {
					nDesSlot2    = READ_DIGITAL(m_nIOPoint+2 , &nIOStatus); 
					nDesStation2 = READ_DIGITAL(m_nIOPoint+3 , &nIOStatus);
				} else {
					nDesStation1 = 0; 
					nDesStation2 = 0;
				}
				return TRUE;
			} else if (SLOT2 == enFingerType) {
				nDesSlot1    = READ_DIGITAL(m_nIOPoint + 2*m_nCapacity  , &nIOStatus);				
				nDesStation1 = READ_DIGITAL(m_nIOPoint + 1 + 2*m_nCapacity , &nIOStatus); 
				if (2 == m_nCapacity) {					
					nDesSlot2    = READ_DIGITAL(m_nIOPoint + 2 + 2*m_nCapacity , &nIOStatus); 
					nDesStation2 = READ_DIGITAL(m_nIOPoint + 3 + 2*m_nCapacity , &nIOStatus);
				} else {
					nDesStation1 = 0; 
					nDesStation2 = 0;
				}
				return TRUE;
			}
		}
	} else if (CASSETTE_TYPE == m_enAreaType) {
		//No need 
		return FALSE;
	} else {
		//Buffer type & pm type
		//Just return enable send slot not source slot
		for (int i = 0 ; i < m_nSlotCount ; i++) {
			if (1 == m_nCapacity) {
				if (TRUE == IsReadyToSend(AUTO , i+1 , 0)) {
					nDesSlot1    = i+1;
					nDesSlot2    = 0; nDesStation1 = 0; nDesStation2 = 0;
					return TRUE;
				}
			} else if(2 == m_nCapacity) {
				if (TRUE == IsReadyToSend(AUTO , i*2+1 , i*2+2)) {
					int nSlot1 = i*2+1; int nSlot2 = i*2+2;
					nDesSlot1    = nSlot1;
					nDesSlot2    = nSlot2;
					nDesStation1 = 0; nDesStation2 = 0;
					return TRUE;
				}
				else {
					if (FALSE == GetDummyMode()) {
						if (TRUE == IsReadyToSend(AUTO, i*2+1 , 0)) {
							nDesSlot1 = i*2+1;
							nDesSlot2 = 0;
							nDesStation1 = 0; nDesStation2 = 0;
							return TRUE;
						}
					}
				}
			}			
		}
	}
	return bRet;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction CCMArea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : 
//Description  : CCMArea Class
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CCMArea::CCMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
: CArea(szAreaName , nSlotCount , nCapacity , nSWEnum , nIOPoint , bDummyMode)
{	
	m_enAreaType = CASSETTE_TYPE;
}

CCMArea::~CCMArea()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCMArea::PrepareForTransfer(char* TrAreaName)
{
	BOOL bRet = TRUE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoGetTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCMArea::DoGetTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPutTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCMArea::DoPutTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction CATMArea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : 
//Description  : CATMArea Class
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CATMArea::CATMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
: CArea(szAreaName , nSlotCount , nCapacity , nSWEnum , nIOPoint , bDummyMode)
{
	m_enAreaType   = TRANSFER_TYPE;
}

CATMArea::~CATMArea()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CATMArea::PrepareForTransfer(char* TrAreaName)
{
	BOOL bRet = TRUE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoGetTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CATMArea::DoGetTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	BOOL bRet = FALSE;
	int  nIOStatus    = 0;
	int  nSlotA , nSlotB;
	char szEvent[256] = {0};
	char szArm[32]    = {0};
	
	do {
		switch (enFingerType) {
		case SLOT1:
			nSlotA = nSlot1;
			nSlotB = 0;
			break;
		case SLOT2:
			nSlotA = 0;
			nSlotB = nSlot2;	
			break;
		case ALL:
			nSlotA = nSlot1;
			nSlotB = nSlot2;
			break;
		case AUTO: 
			return FALSE;
			break;
		}

		if      (PROCESS_TYPE == enType)
			return FALSE;
		else if (BUFFER_TYPE == enType) 
			sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%d|%d|" ,m_szAreaName , nTrAreaName  , nSlotA , nSlotB);
		else if (CASSETTE_TYPE == enType)
			sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%d|%d|" ,m_szAreaName , nTrAreaName  , nSlotA , nSlotB);
		else return FALSE;
		
		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return FALSE;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}
			if (TRUE == bRet) break;
		}
		bRet = TRUE;
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPutTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CATMArea::DoPutTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	BOOL bRet = FALSE;
	int  nIOStatus    = 0;
	int  nSlotA , nSlotB;
	char szEvent[256] = {0};
	char szArm[32]    = {0};

	printf("+++++++++++++++++++++++  DoPutTransfer nSlot1[%d], nSlot2[%d] +++++ \n", nSlot1, nSlot2);	
	do {
		switch (enFingerType) {
		case SLOT1:
			nSlotA = nSlot1;
			nSlotB = 0;
			break;
		case SLOT2:
			nSlotA = 0;
			nSlotB = nSlot2;	
			break;
		case ALL:
			nSlotA = nSlot1;
			nSlotB = nSlot2;
			break;
		case AUTO: 
			return FALSE;
			break;
		}

		if      (PROCESS_TYPE == enType)
			return FALSE;
		else if (BUFFER_TYPE == enType) {
			//must pass aligner slot
			//Modified by jhlee . 2009/09/09

		       //2009.08.24 by mgsong
		       //When Fixed Align then check aligner wafer status if there exist wafer 
		       //must skip this fixed align routine
		    int nAlgnWafer;
			nAlgnWafer = READ_DIGITAL(FM_AL_Wafer_Status , &nIOStatus);	
			if (0 != nSlotA) {
				//2009.08.24 by mgsong
			    if (0 != nAlgnWafer)  return FALSE;
				//2009.08.24 by mgsong

				sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|%d|0|0" , nSlotA );
				WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
				WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
				while (TRUE) {
					if (!WAIT_SECONDS(1)) return FALSE;
					switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
					case SYS_ABORTED : break;
					case SYS_ERROR   : break;
					case SYS_SUCCESS : bRet = TRUE; break;
					}
					if (TRUE == bRet) break;
				}

				bRet = FALSE;
				sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|%d|0|0" , nSlotA );
				WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
				WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
				while (TRUE) {
					if (!WAIT_SECONDS(1)) return FALSE;
					switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
					case SYS_ABORTED : break;
					case SYS_ERROR   : break;
					case SYS_SUCCESS : bRet = TRUE; break;
					}
					if (TRUE == bRet) break;
				}
			}

			bRet = FALSE;
			if (0 != nSlotB) {
				//2009.08.24 by mgsong
			    if (0 != nAlgnWafer)  return FALSE;
				//2009.08.24 by mgsong

				sprintf(szEvent , "MAINT_INTERFACE PLACE_FALIC|1|AL|0|%d|0" ,nSlotB);
				WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
				WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
				while (TRUE) {
					if (!WAIT_SECONDS(1)) return FALSE;
					switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
					case SYS_ABORTED : break;
					case SYS_ERROR   : break;
					case SYS_SUCCESS : bRet = TRUE; break;
					}
					if (TRUE == bRet) break;
				}

				bRet = FALSE;
				sprintf(szEvent , "MAINT_INTERFACE PICKAL_FALIC|1|AL|0|%d|0" , nSlotB);
				WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
				WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
				while (TRUE) {
					if (!WAIT_SECONDS(1)) return FALSE;
					switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
					case SYS_ABORTED : break;
					case SYS_ERROR   : break;
					case SYS_SUCCESS : bRet = TRUE; break;
					}
					if (TRUE == bRet) break;
				}
			}
			sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%d|%d|" ,m_szAreaName , nTrAreaName  , nSlot1 , nSlot2);
		} 
		else if (CASSETTE_TYPE == enType)
			sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%d|%d|" ,m_szAreaName , nTrAreaName  , nSlot1 , nSlot2);
		else return FALSE;
		
		bRet = FALSE;
		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		printf("[Transfer] %s \n",szEvent);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return FALSE;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}
			if (TRUE == bRet) break;
		}
		bRet = TRUE;
	} while (0);
	return bRet;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction CLLArea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : CLLArea Class
//Description  : 
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CLLArea::CLLArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
: CArea(szAreaName , nSlotCount , nCapacity , nSWEnum , nIOPoint , bDummyMode)
{
	m_enAreaType = BUFFER_TYPE;	
}

CLLArea::~CLLArea()
{

}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLLArea::PrepareForTransfer(char* TrAreaName)
{
	BOOL bRet = FALSE;
	int  nIOStatus;
	char szEvent[256] = {0};

	do {
		sprintf(szEvent , "MAINT_INTERFACE WAITING_FOR_%s|1|%s" ,TrAreaName , m_szAreaName);		
		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return FALSE;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}

			if (TRUE == bRet) break;
		}
		bRet = TRUE;
	} while (0);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoGetTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLLArea::DoGetTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPutTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLLArea::DoPutTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction CVTMArea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : 
//Description  : CVTMArea Class
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CVTMArea::CVTMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
: CArea(szAreaName , nSlotCount , nCapacity , nSWEnum , nIOPoint,bDummyMode)
{
	m_enAreaType   = TRANSFER_TYPE;
}

CVTMArea::~CVTMArea()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVTMArea::PrepareForTransfer(char* TrAreaName)
{
	BOOL bRet = TRUE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoGetTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVTMArea::DoGetTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	BOOL bRet = FALSE;
	int  nIOStatus    = 0;
	int  nSelectedArm = 0;
	char szEvent[256] = {0};
	char szArm[32]    = {0};
	
	do {
		switch (enFingerType) {
		case SLOT1:
		case SLOT2:
		case SLOT3:
		case SLOT4:
			nSelectedArm = enFingerType + 1;
			break;
		case ALL:
			return FALSE; break;
		case AUTO: 
			nSelectedArm = GetValidArm();
			break;
		}

		if      (1 == nSelectedArm) strcpy(szArm , "A");
		else if (2 == nSelectedArm) strcpy(szArm , "B");
		else if (3 == nSelectedArm) strcpy(szArm , "C");
		else if (4 == nSelectedArm) strcpy(szArm , "D");
		else  return FALSE;

		if      (PROCESS_TYPE  == enType) {
			if (1 == m_nCapacity)
				sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
			else if (2 == m_nCapacity) {
				if (0 == nSlot2) {
					sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
				}
				else {
					//2014.05.19 Slot 2번으로도 Single Pick 가능 함
					if (0 == nSlot1) {
						//return FALSE;
						//2번 만으로 Single Pick
						int nSingleSlot2 = nSlot2*100;
						sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nSingleSlot2 , nSingleSlot2);
					}
					else {
						int nDoubleSlot = (nSlot2*100) + nSlot1;
						sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nDoubleSlot , nDoubleSlot);
					}
				}
			}
			else return FALSE;
		}
		else if (BUFFER_TYPE   == enType) {
			if (1 == m_nCapacity)
				sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
			else if (2 == m_nCapacity) {
				if (0 == nSlot2) {
					sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
				}
				else {
					//2014.05.19 Slot 2번으로도 Single Pick 가능 함
					if (0 == nSlot1) {
						//return FALSE;
						//2번 만으로 Single Pick
						int nSingleSlot2 = nSlot2*100;
						sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nSingleSlot2 , nSingleSlot2);
					}
					else {
						int nDoubleSlot = (nSlot2*100) + nSlot1;
						sprintf(szEvent , "MAINT_INTERFACE PICK_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nDoubleSlot , nDoubleSlot);
					}
				}
			}
			else return FALSE;
		}
		else if (CASSETTE_TYPE == enType)
			return  FALSE;
		else return FALSE;
		
		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return FALSE;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}
			if (TRUE == bRet) break;
		}
		bRet = TRUE;
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPutTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVTMArea::DoPutTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	BOOL bRet = FALSE;
	int  nIOStatus    = 0;
	int  nSelectedArm = 0;
	char szEvent[256] = {0};
	char szArm[32]    = {0};
	
	
	do {
		switch (enFingerType) {
		case SLOT1:
		case SLOT2:
		case SLOT3:
		case SLOT4:
			nSelectedArm = enFingerType + 1;
			break;
		case ALL:
			return FALSE; break;
		case AUTO: 
			nSelectedArm = PutValidArm();
			break;
		}


		if      (1 == nSelectedArm) strcpy(szArm , "A");
		else if (2 == nSelectedArm) strcpy(szArm , "B");
		else if (3 == nSelectedArm) strcpy(szArm , "C");
		else if (4 == nSelectedArm) strcpy(szArm , "D");
		else  return FALSE;

		if      (PROCESS_TYPE  == enType) {
			if (1 == m_nCapacity)
				sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
			else if (2 == m_nCapacity) {
				if (0 == nSlot2) {
					sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
				}
				else {
					//2014.05.19 Slot 2번으로도 Single Place가 가능 함
					if (0 == nSlot1) {
						//return FALSE;
						//2번 만으로 Single Place
						int nSingleSlot2 = nSlot2*100;
						sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nSingleSlot2 , nSingleSlot2);
					}
					else {
						//1번 만으로 Single Place
						int nDoubleSlot = (nSlot2*100) + nSlot1;
						sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nDoubleSlot , nDoubleSlot);
					}
				}
			}
			else return FALSE;
		}
		else if (BUFFER_TYPE   == enType) {
			if (1 == m_nCapacity)
				sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
			else if (2 == m_nCapacity) {
				if (0 == nSlot2) {
					sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|1|%d" ,m_szAreaName , nTrAreaName , szArm , nSlot1);
				}
				else {
					//2014.05.19 Slot 2번으로도 Single Place가 가능 함
					if (0 == nSlot1) {
						//return FALSE;
						//2번 만으로 Single Place
						int nSingleSlot2 = nSlot2*100;						
						sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nSingleSlot2 , nSingleSlot2);
					}
					else {
						int nDoubleSlot = (nSlot2*100) + nSlot1;
						sprintf(szEvent , "MAINT_INTERFACE PLACE_%s|1|%s|%s|%d|%d" ,m_szAreaName , nTrAreaName , szArm , nDoubleSlot , nDoubleSlot);
					}
				}
			}
			else return FALSE;
		}
		else if (CASSETTE_TYPE == enType)
			return FALSE;
		else return FALSE;
		
		WRITE_DIGITAL(Transfer_Status , 0 , &nIOStatus);
		WRITE_STRING (SCHEDULER , szEvent , &nIOStatus);
		printf("[Transfer] %s \n",szEvent);
		while (TRUE) {
			if (!WAIT_SECONDS(1)) return FALSE;
			switch (READ_DIGITAL(Transfer_Status , &nIOStatus)) {
			case SYS_ABORTED : break;
			case SYS_ERROR   : break;
			case SYS_SUCCESS : bRet = TRUE; break;
			}			
			if (TRUE == bRet) break; 
		}

		bRet = TRUE;
	} while (0);
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : GetValidArm()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int  CVTMArea::GetValidArm()
{
	int i,j,k , nIOStatus , nSlot;
	i = j = k = nSlot = 0;
	for (i=0;i<m_nSlotCount;i++) {
		if (0 >= READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
			nSlot = i+1;
			if (1 == m_nCapacity) break;
		} 
		k++;	
		if (0 != nSlot) {
			if (0 >= READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
				nSlot = i+1; break;
			} else nSlot = 0;
		} 
		k++;
	}
	return nSlot;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PutValidArm()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : int
//////////////////////////////////////////////////////////////////////////////////////////////////////
int  CVTMArea::PutValidArm()
{
	int i,j,k , nIOStatus , nSlot;
	i = j = k = nSlot = 0;

	for (i=0;i<m_nSlotCount;i++) {
		if (0 < READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
			nSlot = i+1;
			if (1 == m_nCapacity || FALSE == GetDummyMode()) break;
		} 
		k++;
		if (0 != nSlot) {
			if (0 < READ_DIGITAL(m_nIOPoint + (k*2) , &nIOStatus)) {
				nSlot = i+1; break;
			} else nSlot = 0;
		} 
		k++;
	}
	return nSlot;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Construction/Destruction CPMArea
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//////////////////////////////////////////////////////////////////////////////////////////////////////
//Date         : 
//Description  : CPMArea Class
//Arguments    :
//////////////////////////////////////////////////////////////////////////////////////////////////////
CPMArea::CPMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode)
: CArea(szAreaName , nSlotCount , nCapacity , nSWEnum , nIOPoint , bDummyMode)
{
	m_enAreaType = PROCESS_TYPE;

}

CPMArea::~CPMArea()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : PrepareForTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPMArea::PrepareForTransfer(char* TrAreaName)
{
	BOOL bRet = TRUE;
	Module_Status nModeSts = SYS_ABORTED;

	if (0 == strcmp(GetAreaName() , "PM1")) {
		nModeSts = RUN_FUNCTION(FUNC_CALL_PM1 , "CHUCKHOME");   //2014.04.10
	} else if (0 == strcmp(GetAreaName() , "PM2")) {
		nModeSts = RUN_FUNCTION(FUNC_CALL_PM2 , "CHUCKHOME");   //2014.04.10
	} else if (0 == strcmp(GetAreaName() , "PM3")) {
		nModeSts = RUN_FUNCTION(FUNC_CALL_PM3 , "CHUCKHOME");   //2014.04.10
	} else bRet = FALSE;

	if (SYS_SUCCESS != nModeSts) bRet = FALSE;
	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoGetTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPMArea::DoGetTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : DoPutTransfer()
//Date         : 2007.06.25
//Description  : 
//Arguments    :
//Return Value : BOOL
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPMArea::DoPutTransfer(char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType)
{
	//Not Transfer Area so return FALSE;
	return FALSE;
}