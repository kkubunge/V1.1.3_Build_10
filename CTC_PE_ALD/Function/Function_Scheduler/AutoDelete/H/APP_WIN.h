#ifndef __APP_WIN__
#define __APP_WIN__

HWND CreateTooltip(HWND hParents, HINSTANCE hInstance, char* szMsg, RECT* prect);
void ChangeToolTipText(HWND ToolHwnd, HWND hParents, HINSTANCE hInstance, char* szMsg );
void Text_Draw_To_DIALOG( HWND hWnd, int x, int y, int fontSize, int fnWeight, COLORREF TextColor, BOOL bTransparency, char* fontName, char* szData );


#endif