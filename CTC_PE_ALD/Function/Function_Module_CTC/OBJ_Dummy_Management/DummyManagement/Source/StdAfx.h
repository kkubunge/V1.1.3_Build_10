// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B07E21B1_E48E_450E_9729_346C7C9E4075__INCLUDED_)
#define AFX_STDAFX_H__B07E21B1_E48E_450E_9729_346C7C9E4075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include "iodefine.h"

#define   MAX_SLOT  25
enum {NO_ALARM , W_POSTED , A_POSTED};
enum {NA , _ERROR , ERROR1 , ERROR2 , ERROR3 , PM1 , PM2 , PM3};
typedef struct {
	long m_enAlarmStatus;
	long m_enOrgMode;
	BOOL m_bChanged;
} DUMMY_INFO;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B07E21B1_E48E_450E_9729_346C7C9E4075__INCLUDED_)
