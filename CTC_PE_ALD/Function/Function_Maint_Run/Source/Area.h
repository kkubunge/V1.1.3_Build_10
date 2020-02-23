// Area.h: interface for the CArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREA_H__64B2CFD9_F402_42E5_A83D_08FEA45BCB84__INCLUDED_)
#define AFX_AREA_H__64B2CFD9_F402_42E5_A83D_08FEA45BCB84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArea  
{
public:
	CArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CArea();

	//{{VIRTUAL
    virtual BOOL PrepareForTransfer(char* TrAreaName) = 0;
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType) = 0;
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType) = 0;
	//}}VIRTUAL

    BOOL  IsReadyToSend    (FingerACTType enFingerType  , int nSlot1 , int nSlot2);
	BOOL  IsReadyToReceive (FingerACTType enFingerType  , int nSlot1 , int nSlot2);
	BOOL  GetValidSend     (char* TrAreaName/*IN*/ , FingerACTType& enFingerType , int& nSlot1 , int& nSlot2);
	BOOL  GetValidReceive  (char* TrAreaName/*IN*/ , FingerACTType& enFingerType , int& nSlot1 , int& nSlot2);

	//Utility func
	BOOL  GetCurWaferInfo  (FingerACTType& enFingerType , int& nDesStation1 , int& nDesStation2 , int& nDesSlot1 , int& nDesSlot2 , BOOL& bStepMode);


	char* GetAreaName      ();
	int   GetSWEnum        ();
	int   GetMaxWafer      ();
	AREA_TYPE IsTypeOfArea();
	void  SetBMMode(int& nSlot , int& nMode);

	BOOL  GetDummyMode    ();

	
protected:
	char  m_szAreaName[32];
	int   m_nSlotCount;
	int   m_nCapacity;
	int   m_nSWEnum;
	int   m_nIOPoint;
	AREA_TYPE m_enAreaType;

	BOOL  m_bDummyMode;

private:
	int m_nBMMode[MAX_BM_SLOT];

};

class CCMArea : public CArea  
{
public:
	CCMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CCMArea();

	//{{VIRTUAL
	virtual BOOL PrepareForTransfer(char* TrAreaName);
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	//}}VIRTUAL
};

class CATMArea : public CArea  
{
public:
	CATMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CATMArea();  

	//{{VIRTUAL
	virtual BOOL PrepareForTransfer(char* TrAreaName);
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	//}}VIRTUAL

};

class CLLArea : public CArea  
{
public:
	CLLArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CLLArea();  

	//{{VIRTUAL
	virtual BOOL PrepareForTransfer(char* TrAreaName);
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	//}}VIRTUAL

};

class CVTMArea : public CArea  
{
public:
	CVTMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CVTMArea(); 
	
	//{{VIRTUAL
	virtual BOOL PrepareForTransfer(char* TrAreaName);
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	//}}VIRTUAL

	int  GetValidArm     ();
	int  PutValidArm     ();

};

class CPMArea : public CArea  
{
public:
	CPMArea(char* szAreaName , int nSlotCount , int nCapacity , int nSWEnum , int nIOPoint , BOOL bDummyMode);
	virtual ~CPMArea();

	//{{VIRTUAL
	virtual BOOL PrepareForTransfer(char* TrAreaName);
	virtual BOOL DoGetTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	virtual BOOL DoPutTransfer     (char* nTrAreaName , int nSlot1 , int nSlot2 , AREA_TYPE enType , FingerACTType enFingerType);
	//}}VIRTUAL
};

#endif // !defined(AFX_AREA_H__64B2CFD9_F402_42E5_A83D_08FEA45BCB84__INCLUDED_)
