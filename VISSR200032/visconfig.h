//#include "stdafx.h"
#ifndef VISCONFIG_H
#define VISCONFIG_H

class ConfigIni
{
	HINSTANCE hLib;
	char* LibIni;
	char* Section;
public:
	ConfigIni();
	~ConfigIni();
	BOOL Initialize(char* LibIni, char* Section, HWND parent, HINSTANCE hLib);
	static void setCheck(DWORD idc, BOOL check, HWND hwnd);
	static LPSTR getCheck(DWORD idc, HWND hwnd, LPSTR val, int vsize);
	friend BOOL CALLBACK CONFIG(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};
/*Log 05/15/2023 -- changed all char* to wchar_t for configuration purposes, but found only necessary in unicode char set - Updated Log 9/6/23*/
#endif /* VISCONFIG_H */