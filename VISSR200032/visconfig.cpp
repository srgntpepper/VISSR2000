#include "stdAfx.h"

static ConfigIni* TheConfig;

BOOL CALLBACK CONFIG(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG:

		GetPrivateProfileString(TheConfig->Section, "DebugMode", "0", tbuf, sizeof(tbuf), TheConfig->LibIni);
		ConfigIni::setCheck(IDC_DEBUG, (BOOL)atoi(tbuf), hwnd);

		SetFocus(GetDlgItem(hwnd, IDOK));
		break;

	case WM_COMMAND:
	{
		switch (LOWORD(wp))
		{
		case IDOK:
		{
			
			ConfigIni::getCheck(IDC_DEBUG, hwnd, tbuf, sizeof(tbuf));
			WritePrivateProfileString(TheConfig->Section, "DebugMode", tbuf, TheConfig->LibIni);
			EndDialog(hwnd, TRUE);
		}
		break;
		case IDCANCEL:
		{
			EndDialog(hwnd, 0);
		}
		}
	}
	}
	return 0;
}

//These ctors are part of the dll process and are not really used in this implementation
ConfigIni::ConfigIni()
{
}

ConfigIni::~ConfigIni()
{
}

BOOL ConfigIni::Initialize(char* LibIni, char* Section, HWND parent, HINSTANCE hLib)
{
	this->hLib = hLib;
	this->LibIni = LibIni;
	this->Section = Section;
	TheConfig = this;
	BOOL result = (BOOL)DialogBox(hLib, "config", parent, (DLGPROC)::CONFIG);

	return result;
}

void ConfigIni::setCheck(DWORD idc, BOOL check, HWND hwnd)
{
	SendDlgItemMessage(hwnd, idc, BM_SETCHECK, check ? BST_CHECKED : BST_UNCHECKED, 0);
}

LPSTR ConfigIni::getCheck(DWORD idc, HWND hwnd, LPSTR val, int vsize)
{
	strcpy_s(val, vsize, (SendDlgItemMessage(hwnd, idc, BM_GETCHECK, 0, 0) == BST_CHECKED) ? "1" : "0");
	return val;
}
