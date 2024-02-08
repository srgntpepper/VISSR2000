/***************************************************************************
	NAME : VISSR200032.CPP
	DESC : JETEC VIS Driver for KEYENCE SR2000 BARCODE SCANNER
***************************************************************************/
#include "stdAfx.h"
#include "dirent.h"

/***************************************************************************
	OS INTERFACE FUNCTION PROTOTYPES
***************************************************************************/
//BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

/***************************************************************************
	VIS API Prototypes
***************************************************************************/
extern "C" _declspec (dllexport) PVISINFO VISGetInfo(void);
extern "C" _declspec (dllexport) BOOL VISOpen(HWND parent, int id, HWND * adwnd);
extern "C" _declspec (dllexport) void VISClose(void);
extern "C" _declspec (dllexport) void VISPoll(void);
extern "C" _declspec (dllexport) void VISPrep(LPSTR prepstring);
extern "C" _declspec (dllexport) void VISGetPrepNames(LPSTR prepnames);
extern "C" _declspec (dllexport) void VISInspect(void);
extern "C" _declspec (dllexport) void VISRead(void);
extern "C" _declspec (dllexport) void VISLive(BOOL enable);
extern "C" _declspec (dllexport) void VISLocate(HWND xywnd);
extern "C" _declspec (dllexport) void VISXYMsg(WORD xyn);
extern "C" _declspec (dllexport) void VISCancel(void);
extern "C" _declspec (dllexport) BOOL VISConfigIni(HWND parent);

/***************************************************************************
	CALPOPUP DIALOG INTERFACE PROTOTYPES
***************************************************************************/
BOOL CALLBACK _CALPOPUPDP(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP);
static void openCalPopup();
static void closeCalPopup();
static void SaveWinRect(HWND hwnd);

/***************************************************************************
	GLOBAL VARIABLES
***************************************************************************/
HINSTANCE hLib;								// Libraries module handle
char LibName[] = "VISSR200032";				// Library name
char IniSection[] = "VISSR200032";			// INI section name
char LibIni[MAX_PATH];						// Path to the ini file


///* Conversions to make certain variables LPCWSTR compatible in unicode char set */
//wchar_t LibNameWide[] = L"VISSR200032";		// Library name
//wchar_t IniSectionWide[] = L"VISSR200032";	// INI section name
//wchar_t LibIniWide[MAX_PATH];					// Path to the ini file
//wchar_t tbufWide[128];						// Handy general use text buf

HWND* pADWnd = NULL;						// Apps ADWnd
int SettlingTime;							// Settling Time in ms

SR2000DEV dev;								// The SR2000 device

char tbuf[128];								// Handy general use text buf
WORD tlen;									// Length of data in tbuf

BOOL DebugMode = FALSE;						// Debugflag set in ini file
char dbuf[128];								// Handy text buffer used for debug messages

BOOL injog = FALSE;
BOOL inmove = FALSE;						///SHOULD BE STATE EXCLUSIVE WITH INJOG + UNKNOWN READY ERROR
ConfigIni VISConfig;

HWND hWndCalPopup = 0;
COLORREF liveColor = RGB(255, 255, 0);
COLORREF normColor = RGB(192, 192, 192);
HBRUSH liveBrush;
HBRUSH normBrush;

// Declare a boolean flag to track if the DLL has been initialized
BOOL isInitialized = false;

// Function to perform the initialization tasks
void InitializeDLL()
{
	// Ensure initialization is done only once
	if (isInitialized)
		return;

	// Get the module handle of the DLL
	hLib = GetModuleHandle(NULL);

	// Retrieve the DLL's file path
	char dllPath[MAX_PATH];
	GetModuleFileNameA(hLib, dllPath, MAX_PATH);

	// Convert the DLL file path to a narrow string - for unicode
	//std::wstring dllPathWide(dllPath);
	//std::string dllPathStr(dllPath.begin(), dllPath.end());

	std::string dllPathStr(dllPath);

	// Find the position of the last backslash (\) in the DLL file path
	size_t lastBackslashPos = dllPathStr.find_last_of('\\');
	if (lastBackslashPos != std::string::npos)
	{
		// Remove the DLL file name from the path
		std::string iniPathStr = dllPathStr.substr(0, lastBackslashPos + 1);

		// Append the INI file name with .ini extension
		iniPathStr += "VISSR200032.ini";

		// Convert the INI file path back to a wide string - for unicode only
		//std::wstring iniPathWide(iniPathStr.begin(), iniPathStr.end());

		// Assign the wide string to LibIniWide
		strncpy_s(LibIni, iniPathStr.c_str(), MAX_PATH - 1);
		LibIni[MAX_PATH - 1] = L'\0';

		// Display the INI file path for testing
		//MessageBoxA(NULL, iniPathStr.c_str(), "INI File Path", MB_OK | MB_ICONINFORMATION);
	}

	DebugMode = GetPrivateProfileInt(IniSection, "DebugMode", 0, LibIni);
	SettlingTime = GetPrivateProfileInt(IniSection, "SettlingTime", 3000, LibIni);
	SettlingTime = max(SettlingTime, 100);
	SettlingTime = min(SettlingTime, 5000);
	liveBrush = CreateSolidBrush(liveColor);
	normBrush = CreateSolidBrush(normColor);

	// Set the initialization flag to indicate that initialization is complete
	isInitialized = true;
}

/***************************************************************************
	OS INTERFACE FUNCTIONS
***************************************************************************/
/***************************************************************************
	ok = DllMain(hModule,wDataSeg,cbHeapSize,lpszCmdLine)

	Preforms Initialization of the DLL.
	Called by Windows when the DLL is loaded.

	int ok = TRUE indicates success, 0 indicates failure.
	HANDLE hModule = Module handle of library.
***************************************************************************/
/*
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hLib = (HINSTANCE)hModule;

		InitializeDLL();
		
		return TRUE;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		hLib = NULL;
		DeleteObject(normBrush);
		DeleteObject(liveBrush);
		return TRUE;
	}
	return TRUE;
}
*/


/***************************************************************************
	API EXPORT FUNCTIONS
***************************************************************************/
extern "C" _declspec (dllexport) BOOL VISConfigIni(HWND parent)
{
	InitializeDLL();
	return VISConfig.Initialize(LibIni, LibName, parent, (HINSTANCE)hLib);
}

/******************************************************************************
	pvisinfo=VISGetInfo()
	Returns API informatin/access structure and initializes
******************************************************************************/
extern "C" _declspec (dllexport) PVISINFO VISGetInfo(void)
{
	dev.DevInfo.apiver = VISAPIVER;
	dev.DevInfo.ver = 0x0200;       // version 2.00
	dev.DevInfo.ErrorText = NULL;
	dev.DevInfo.ReadData = NULL;
	dev.DevInfo.nLocatePoints = 0;
	dev.DevInfo.LocatePoints = NULL;
	return &dev.DevInfo;
}

/***************************************************************************
	ok = VISOpen(parent,id,adwnd)

	Connects the VIS system.

	BOOL ok = TRUE on success.
	HWND parent = Parent window handle.
	int id = IDC used by parent to represent this system.
***************************************************************************/
extern "C" _declspec (dllexport) BOOL VISOpen(HWND parent, int id, HWND * adwnd)
{
	InitializeDLL();
	pADWnd = adwnd;

	if (DebugMode)
		OutputDebugString("VISOpen()\n");

	BOOL ok = dev.open(id, parent, LibIni, LibName);
	if (!ok)
	{
		dev.DevInfo.ErrorText = dev.emsg;
		//		MessageBox(NULL,dev.emsg,"VISOpen() FAILED!",MB_OK);
		// Display message box indicating whether isOpen is true or false
		/*
		std::wstring status = L"VISOpen() FAILED!\n";
		MessageBox(NULL, status.c_str(), L"VISOPEN", MB_OK | MB_ICONINFORMATION);
		*/
		if (DebugMode)
			OutputDebugString("VISOpen() FAILED!\n");
		return FALSE;
	}
	

	ok = dev.SrClientSocket_Connect();

	if (DebugMode) 
		{
		std::string debugStr = "VISOpen() = " + std::string(ok ? "TRUE" : "FAILED") + "\n";
		OutputDebugString(debugStr.c_str());

		if (!ok) 
			{
			return FALSE;
			}
		}

	/*
	std::wstring status = L"VISOpen() Returning true!\n";
	MessageBox(NULL, status.c_str(), L"VISOPEN", MB_OK | MB_ICONINFORMATION);
	*/
	return TRUE;
}

/***************************************************************************
	VISClose()

	Close the comm port.
***************************************************************************/
extern "C" _declspec (dllexport) void VISClose(void)
{
	dev.SrClientSocket_Disconnect();
	if (DebugMode)
		OutputDebugString("VISClose()\n");
}

/***************************************************************************
	VISPoll()

	VIS backround polling routine.
	This should be called frequently from either a timer or in a
	peekmessage loop.
***************************************************************************/
extern "C" _declspec (dllexport) void VISPoll(void)
{
}

/***************************************************************************
	VISPrep()

	Parse the prepstring and send commands.
***************************************************************************/
extern "C" _declspec (dllexport) void VISPrep(LPSTR prepstring)
{
	if (DebugMode)
	{
		//we need to convert prepString from LPSTR to LPCWSTR - for unicode only
		//std::wstring prepStringWide = ConvertToLPCWSTR(prepstring);

		OutputDebugString("VISPrep(");
		OutputDebugString(prepstring);
		OutputDebugString(")\n");
	}
	dev.prep(prepstring);
}

/***************************************************************************
	VISGetPrepNames(char VisDirectory)

	Returns the CSV list of availiable prep/object names
***************************************************************************/
extern "C" _declspec (dllexport) void VISGetPrepNames(LPSTR VisDirectory)
{
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(VisDirectory)) != NULL) {
		/* Search all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			printf("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		//	return EXIT_FAILURE;
	}
}

/***************************************************************************
	VISInspect()

	Start the inspection process.
***************************************************************************/
extern "C" _declspec (dllexport) void VISInspect(void)
{
	if (DebugMode)
		OutputDebugString("VISInspect()\n");
	dev.Notify(VISN_SNAP);
	dev.SrClientSocket_Inspect();
}

/***************************************************************************
	VISRead()

	Start the read process.
***************************************************************************/
extern "C" _declspec (dllexport) void VISRead(void)
{
	if (DebugMode)
		OutputDebugString("VISRead()\n");
	dev.Notify(VISN_SNAP);
	dev.SrClientSocket_Read();
}

/***************************************************************************
	VISLive()

	Either turn on or off trigger.
***************************************************************************/
extern "C" _declspec (dllexport) void VISLive(BOOL enable)
{
	if (enable)
	{
		/////////////////////////
		openCalPopup();
		//dev.startLiveLocate();///
		/////////////////////////
		dev.SrClientSocket_Test();
		if (DebugMode)
			OutputDebugString("VISLive(on)\n");
	}
	else
	{
		dev.SrClientSocket_Quit_Test();
		/////////////////////////
		//dev.killLiveLocate(); ///
		closeCalPopup();
		/////////////////////////
		if (DebugMode)
			OutputDebugString("VISLive(off)\n");
			
	}
}

/***************************************************************************
	STARTS JOG LOCATION
***************************************************************************/
extern "C" _declspec (dllexport) void VISLocate(HWND xywnd)
{
	if (DebugMode)
		OutputDebugString("VISLocate()\n");
	dev.xywnd = xywnd;
	SendMessage(xywnd, XYM_JOG, 0, 0);
		

	/////////////////////////
	//if (dev.optCal)					//Chris Davis 02/07/2024 --This may not apply since trig&parse func is not used
	/////////////////////////
	dev.xywnd = xywnd;
	if (dev.optJog)
		openJog();
	else
	{
		dev.getXYPos();
		dev.Notify(VISN_SNAP);
		dev.syncProgTrigRead(SR2000DEV::xLocate);
	}

	/*
	--------------------------------------------------
	Locator1 CAL Locate
		Prep:-p004 -c
		startCalPopup (due to -c)
		:CalPopup:SnapButton
			killLiveLocate
			Locate
			Locate:Found
				>newloc
			Locate:NotFound
				>noloc
		:calPopup:LiveButton
			startLiveLocate
		:calPopup:StopButton
			killLiveLocate
		:calPopup:JogButton
			startJog

		:LiveLocate:Found
			>newloc
		:LiveLocate:NotFound
			>noloc


		:newloc
			camPos=CAM PIXEL POS
		:noloc
			camPos=unknown
		:xyJogComplete
			xyPos=XY POS
			if(camPos)
				wcsDelta=
				wcsLocate=
		:calPopup:UpdateCalButton
		:calPopup:OkButton (completes locate and closes)
		:calPopup:CancelButton (cancles locate and closes)
	--------------------------------------------------
	Locator1 Locate
		Prep:-p004
		Locate
			IF FOUND
				camPos=CAM PIXEL POS
				>LocateComplete
			ELSE
				startJog
				startLiveLocate

			ON LiveLocate
				IF FOUND
					camPos=CAM PIXEL POS
				ELSE
					camPos=UNKNOWN

			ON JOG COMPLETE
				if(camPos)
					>LocateCompete

		:LocateCompete
			wcsDelta=c2w(camPos)-c2w(refPos)
			wcsLocate=wcsJog+wcsDelta
			// IF Z CHANGED INVALID CAL


	--------------------------------------------------
	*/
}

// VISXYMsg accepts messages from xy table during locate jog

extern "C" _declspec (dllexport) void VISXYMsg(WORD xyn)
{
	
	switch (xyn)
	{
	case XYN_JOG:
		///openCalPopup();///
		dev.startLiveLocate();///
		injog = TRUE;
		break;
	case XYN_MOVE:
		inmove = TRUE;
		///seq
		break;
	case XYN_READY:
		if (injog)
		{
			injog = false;
			dev.getXYPos();
			dev.killLiveLocate();
			///seq
			if (dev.mode == SR2000DEV::xCal)
			{
				dev.syncProgTrigRead(SR2000DEV::xCal);
				break;
			}
			Sleep(50);
			dev.syncProgTrigRead(SR2000DEV::xLocatePostJog);
		}
		if (inmove)
		{
			inmove = false;
			///seq
			if (dev.mode == SR2000DEV::xCal)
			{
				Sleep(SettlingTime);
				dev.syncProgTrigRead(SR2000DEV::xCal);
			}
		}
		break;
	case XYN_ERROR:
		if (injog)
		{
			injog = false;
			dev.killLiveLocate();
			dev.DevInfo.ErrorText = "XY Error!";
			dev.Notify(VISN_ERROR);
		}
		if (inmove)
		{
			///seq
			inmove = false;
		}
		break;
	}

}


// VISCancel cancels any mode device is in
extern "C" _declspec (dllexport) void VISCancel(void)
{
	closeJog();
	dev.killLiveLocate();
	dev.SrClientSocket_Loff();
	dev.SrClientSocket_Quit_Test();
	closeCalPopup();
}





/***************************************************************************
	CALPOPUP DIALOG INTERFACE
	
	NOTE:Almost, if not all of the functions below were applicable to a
		 cameral device that was used in visifmo2d.  That is the vis file
		 this dll was built off of.
***************************************************************************/

BOOL CALLBACK _CALPOPUPDP(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP)
{
	
	RECT r;
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		char calPopupWinRect[] = "CalPopupWinRect";
		if (GetPrivateProfileIntVect(LibName, calPopupWinRect, (LPINT)&r, 4, LibIni))
			SetWindowPos(hwnd, 0, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		dev.camcal.glPrep(hwnd);
		return true;
	}
	case WM_SETFOCUS:
		if (wP)
			SetFocus((HWND)wP);
		return false;
	case WM_COMMAND:
		switch (LOWORD(wP))
		{
		case IDC_CALNOW: // at current position
			///ZZZ MOVE TEST
			if (dev.xywnd && !injog)
			{
				///seq
				dev.calIndex = 0;
				dev.syncProgTrigRead(SR2000DEV::xCal);
			}
			return true;
		case IDC_CALLIVE:
			/// IF IN LIVE LOCATE
			if (dev.isLiveLocateRunning())
				dev.killLiveLocate();
			else
				dev.startLiveLocate();
			/// UPDATE BUTTON TO REFLECT STATE
			return true;
		case IDC_CALSNAP:
			if (dev.isLiveLocateRunning())
				dev.killLiveLocate();
			else
				dev.syncProgTrigRead(SR2000DEV::xSnap);
			return true;
		case IDC_CALSAVE:
			dev.camcal.savesolution();
			return true;
		case IDC_CALRELOAD:
			dev.camcal.loadsolution();
			InvalidateRect(hWndCalPopup, NULL, true);
			return true;
		}
		return false;
	case WM_CTLCOLOREDIT:
		SetBkColor((HDC)wP, dev.isLiveLocateRunning() ? liveColor : normColor);
		return dev.isLiveLocateRunning() ? (BOOL)liveBrush : (BOOL)normBrush;
	case WM_SIZE:
	case WM_MOVE:
		SaveWinRect(hwnd);
		return false;
	case WM_CLOSE:
		closeCalPopup();
		break;
	case WM_DESTROY:
		dev.camcal.glDone();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		dev.camcal.glDraw(ps.hdc);
		EndPaint(hwnd, &ps);
	}
	return true;
	}
	return false;
}


static void openCalPopup()
{
	if (!hWndCalPopup)
	{
		hWndCalPopup = CreateDialog(hLib, "CALPOPUP", NULL, _CALPOPUPDP);
		ShowWindow(hWndCalPopup, SW_SHOWNA);
	}
}



static void closeCalPopup()
{
	if (hWndCalPopup)
		DestroyWindow(hWndCalPopup);
	hWndCalPopup = 0;
}


static void SaveWinRect(HWND hWnd)
{
	
	RECT WinRect;
	GetWindowRect(hWnd, &WinRect);

	WinRect.left = max(WinRect.left, 0);
	WinRect.top = max(WinRect.top, 0);

	wsprintf(tbuf, "%d,%d,%d,%d",
		WinRect.left, WinRect.top, WinRect.right, WinRect.bottom);
	WritePrivateProfileString(LibName, "CalPopupWinRect", tbuf, LibIni);
	
}


void setCalPopupInfo(char* txt)
{
	//for unicode only
	//std::wstring wtxt = ConvertToLPCWSTR(txt);

	if (hWndCalPopup)
		SetDlgItemText(hWndCalPopup, IDC_CALINFO, txt);
}


void openJog()
{
	if (!injog)
		SendMessage(dev.xywnd, XYM_JOG, 0, 0);
}

void closeJog()
{
	if (injog)
		SendMessage(dev.xywnd, XYM_JOG, 0, 0);
}



