// PersistIO.h: interface for the CPersistIO class.
// Create : 2006.03.20
// Last Update : 2006.12.06
// Writer : Chang-su, Kim.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERSISTIO_H__22A24493_466C_4E7B_99FA_DF6B99F3573B__INCLUDED_)
#define AFX_PERSISTIO_H__22A24493_466C_4E7B_99FA_DF6B99F3573B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
#include "MonThread.h"

class CPersistIO  : protected CDynamicIO, protected CMonThread
{
public:
	CPersistIO();
	virtual ~CPersistIO();

// Member Function
public:
	bool Initialize(char *szConfigPath = NULL);
	void Deinitialize();
	bool SaveIOList();
private:
	bool ReadIOList();
	bool UpdateIOList();
	bool ReadValueFromIO();
	bool WriteIOValueToFile();
	int GetIOType(char *szType);
	char GetIOTypeChar(int nType);

// Virtual Function
protected:
	virtual void DoMonitor();

// Member Variable
private:
	#define MAX_IO_LIST		200
	IO_LIST	m_pIOList[MAX_IO_LIST+1];	// 2006.08.21
	char m_pIOValue[MAX_IO_LIST][100];  //2013.08.23
	char m_szConfigPath[80];
	BOOL m_abValueInit[MAX_IO_LIST];		// 2009.11.03

// Comment Processer
private:
	void ClearComment();
	void InitComment();
	void SetComment(int nIndex, char *szComment);
	char *GetComment(int nIndex);
	#define MAX_COMMENT		40
	#define MAX_CMT_CHAR	80
	typedef struct COMMENT_STRUCT
	{
		char szComment[MAX_CMT_CHAR];
		int nIndex;
	};
	COMMENT_STRUCT m_aCmtList[MAX_COMMENT];
	int m_nCommentCount;
};

#endif // !defined(AFX_PERSISTIO_H__22A24493_466C_4E7B_99FA_DF6B99F3573B__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2006.03.20	Created
			최대 I/O 정의 가능 갯수는 100개 임.
			String Channel의 경우 글자수는 39자를 넘으면 않됨.
			Comment 는 80자 이하 이어야 하며 40 Line 까지 쓸수 있다.
2006.03.21	IOList 초기화 Fail시에도 정상 I/O에 대해서는 Update 까지
			진행되도록 수정.
2006.08.21	IOList Instance 갯수를 사용갯수보다 1개 늘림.
			CDynamicIO class에서 마지막 Channel 인지를 Instance의 Name
			가지고 인식하기 때문.
2006.12.06	Real I/O 에 대하여 I/O 읽기 Fail시에 변경 값이 적용되지 않도록
			수정함.
			class 소멸자에 Channel Reading Routine 제거
2006.12.20	I/O Monitoring 주기를 더 빨리 하도록 수정
			Function을 따로 분리하므로 인해 이제부터 Version 관리 File을 따로 관리함.
*/
