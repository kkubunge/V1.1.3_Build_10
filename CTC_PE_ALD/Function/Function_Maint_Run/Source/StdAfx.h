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

//Test
#ifndef _DEBUG
	#define _DEBUG
#endif

#define MAX_BM_SLOT	 26

enum {_INOUT , _IN , _OUT};
typedef enum {SLOT1 = 0 , SLOT2 , SLOT3 , SLOT4 , AUTO , ALL} FingerACTType;
typedef enum {CASSETTE_TYPE = 0 , BUFFER_TYPE , PROCESS_TYPE , TRANSFER_TYPE} AREA_TYPE;
        

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B07E21B1_E48E_450E_9729_346C7C9E4075__INCLUDED_)
