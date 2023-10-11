#include "stdAfx.h"
#include "visconfig.h"
#include "vissr200032.h"

static ConfigIni* TheConfig;

BOOL CALLBACK CONFIG(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	///	HWND port=GetDlgItem(hwnd,IDC_PORT);
	///	HWND baudrate=GetDlgItem(hwnd,IDC_BRATE);
	switch (msg)
	{
	case WM_INITDIALOG:
		///			GetPrivateProfileString(Section,"Port","COM1,9600,E,7,2",tbuf,sizeof(tbuf),IniPath);
		///			ConfigIni::iniPortSettings(hwnd,tbuf);
		///			GetPrivateProfileString(Section,"Start","83",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_START,tbuf);
		///			GetPrivateProfileString(Section,"Stop","69",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_STOP1,tbuf);
		///			GetPrivateProfileString(Section,"StartChar","2",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_STARTCHAR,tbuf);
		///			GetPrivateProfileString(Section,"EndChar","13",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_ENDCHAR,tbuf);
		///			GetPrivateProfileString(Section,"ReadTimeOut","2000",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_READTO,tbuf);
		///			GetPrivateProfileString(Section,"InspectTimeoutError","0",tbuf,sizeof(tbuf),IniPath);
		///			::SetDlgItemText(hwnd,IDC_INSPTO,tbuf);
		///			GetPrivateProfileString(Section,"TestMode","0",tbuf,sizeof(tbuf),IniPath);
		///			ConfigIni::setCheck(IDC_TESTM,(BOOL)atoi(tbuf),hwnd);

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
			///					ConfigIni::savPortSettings(hwnd,tbuf);
			///					WritePrivateProfileString(Section,"Port",tbuf,IniPath);
			///					::GetDlgItemText(hwnd,IDC_START,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"Start",tbuf,IniPath);
			///					::GetDlgItemText(hwnd,IDC_STOP1,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"Stop",tbuf,IniPath);
			///					::GetDlgItemText(hwnd,IDC_STARTCHAR,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"StartChar",tbuf,IniPath);
			///					::GetDlgItemText(hwnd,IDC_ENDCHAR,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"EndChar",tbuf,IniPath);
			///					::GetDlgItemText(hwnd,IDC_READTO,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"ReadTimeOut",tbuf,IniPath);
			///					/* NO VALUE FOR LEARN TIME OUT?
			///					::GetDlgItemText(hwnd,IDC_LEARNTO,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"LearnTimeOut",tbuf,IniPath);
			///					*/
			///					::GetDlgItemText(hwnd,IDC_INSPTO,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"InspectTimeoutError",tbuf,IniPath);
			/// 
			
			ConfigIni::getCheck(IDC_DEBUG, hwnd, tbuf, sizeof(tbuf));
			WritePrivateProfileString(TheConfig->Section, "DebugMode", tbuf, TheConfig->LibIni);
			///					ConfigIni::getCheck(IDC_DUMPM,hwnd,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"DumpMode",tbuf,IniPath);
			///					ConfigIni::getCheck(IDC_TESTM,hwnd,tbuf,sizeof(tbuf));
			///					WritePrivateProfileString(Section,"TestMode",tbuf,IniPath);
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

	/*
	// Display message box with variable contents
	std::wstring message = L"hLib: " + std::to_wstring(reinterpret_cast<int>(hLib)) + L"\n"
		+ L"LibIni: " + std::wstring(LibIni) + L"\n"
		+ L"Section: " + std::wstring(Section) + L"\n"
		+ L"Result: " + (result ? L"True" : L"False");

	MessageBox(NULL, message.c_str(), L"Variable Contents", MB_OK | MB_ICONINFORMATION);
	*/

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
