#pragma once
#include "SR2000Dev.h"
#include "visconfig.h"

/***************************************************************************
	NAME : VISSR200032.H
	DESC : GLOBALS for VISSR200032
***************************************************************************/

/***************************************************************************
	GLOBAL VARIABLES
***************************************************************************/
extern HINSTANCE hLib;
extern char LibName[];
extern char IniSection[];
extern char LibIni[];
extern HWND* pADWnd;
extern int SettlingTime;

//wide versions added for unicode character set - Chris Davis
//extern wchar_t LibNameWide[];		// Library name
//extern wchar_t IniSectionWide[];  // INI section name
//extern wchar_t LibIniWide[];
//extern wchar_t tbufWide[128];

extern COLORREF liveColor;
extern COLORREF normColor;
extern HBRUSH liveBrush;
extern HBRUSH normBrush;

extern SR2000DEV dev;

extern char tbuf[128];
extern WORD tlen;

extern BOOL DebugMode;
extern char dbuf[128];

extern BOOL injog;
extern ConfigIni VISConfig;

extern HWND hWndCalPopup;
//extern void setCalPopupInfo(char* txt);

extern void openJog();
extern void closeJog();

