/*=======================================================================================
//=======================================================================================
// COPYRIGHT 1999/2001 BY  KORNIC SYSTEM CORPORATION.
//
//     ----------------+     128-2, Chuckbuk-Ri,
//    |             ///|     Nami-Myun, Chungwon-Gun,
//    |         /////  |     Chung-Buk, 363-810, Korea
//    |     ///////    |
//    |     //////     |     Tel: 82-43-269-4895
//    |    ///////     |     Fax: 82-43-269-4898
//    |  /////         |
//    |///             |     E-Mail: support@easycluster.co.kr
//    +----------------      http://www.easycluster.co.kr
//=======================================================================================
//
//####################################################################################################
// IDENTIFICATION											FN : ExecLib.h
//####################################################################################################
// 
// PROGRAM-ID	: Easy Cluster Excel Support Library.
// VERSION		: Original (Ver. 021203)
// AUTHOR		: Y-N Song. (ynsong@kornic.co.kr)
// COMPANY		: Kornic Systems Co., Ltd.
// Release Date : 2002-12-03
//
//####################################################################################################
*/

#include <ole2.h>
#include <stdio.h>
#include <iostream.h>

//####################################################################################################
// GLOBAL Variable
//####################################################################################################


//####################################################################################################
// GLOBAL Function
//####################################################################################################
HRESULT InitExcel();
HRESULT Add_Workbooks();
HRESULT QUIT_WorkBook();
HRESULT Get_Excel_CLSID();
HRESULT	Get_Excel_Instance();
HRESULT Get_ActiveSheet_Object();
HRESULT Get_Workbooks_Collection();
HRESULT Show_Excel_Application(BOOL bVisible);
HRESULT Get_Excels_CLSID(CLSID* Excel_ID);
HRESULT Set_Range(WCHAR* pszStart, WCHAR* pszEnd);
HRESULT	SAVE_WorkBook(WCHAR* wpsz_Excel_FileName);
HRESULT Show_Save_AlertMessage(BOOL bAlertVisible);

HRESULT Put_Bool(WCHAR* pwszCell, BOOL bVal);
HRESULT Put_Real(WCHAR* pwszCell, float rVal);
HRESULT Put_Short(WCHAR* pwszCell, short dsValue);
HRESULT Put_Double(WCHAR* pwszCell, double dwVal);
HRESULT Put_Integer(WCHAR* pwszCell, int dValue);
HRESULT Put_String(WCHAR* pwszCell, WCHAR* pszMsg);

HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs,...) ;
void Dispose_Memory();
