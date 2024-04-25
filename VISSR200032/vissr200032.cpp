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
//extern "C" _declspec (dllexport) void VISGetPrepNames(LPSTR prepnames);
extern "C" _declspec (dllexport) void VISInspect(void);
extern "C" _declspec (dllexport) void VISRead(void);
extern "C" _declspec (dllexport) void VISLive(BOOL enable);
extern "C" _declspec (dllexport) void VISLocate(HWND xywnd);
extern "C" _declspec (dllexport) void VISXYMsg(WORD xyn);
extern "C" _declspec (dllexport) void VISCancel(void);
extern "C" _declspec (dllexport) BOOL VISConfigIni(HWND parent);


/***************************************************************************
	GLOBAL VARIABLES
***************************************************************************/
HINSTANCE hLib;								// Libraries module handle
char LibName[] = "VISSR200032";				// Library name
char IniSection[] = "VISSR200032";			// INI section name
char LibIni[MAX_PATH];						// Path to the ini file

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

	// Set the initialization flag to indicate that initialization is complete
	isInitialized = true;
}

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

	return TRUE;
}

/***************************************************************************
	VISClose()

	Close the communication socket.
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
	**NOTE: this method should only be applicable in serial communication**
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
		//for unicode only - we need to convert prepString from LPSTR to LPCWSTR
		//std::wstring prepStringWide = ConvertToLPCWSTR(prepstring);

		OutputDebugString("VISPrep(");
		OutputDebugString(prepstring);
		OutputDebugString(")\n");
	}
	dev.prep(prepstring);
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
		dev.SrClientSocket_Test();
		if (DebugMode)
			OutputDebugString("VISLive(on)\n");
	}
	else
	{
		dev.SrClientSocket_Quit_Test();
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
	dev.Notify(VISN_SNAP);
	dev.SrClientSocket_Test();
	dev.xywnd = xywnd;
	SendMessage(dev.xywnd, XYM_JOG, 0, 0);
}

// VISXYMsg accepts messages from xy table during locate jog
extern "C" _declspec (dllexport) void VISXYMsg(WORD xyn)
{
	
	switch (xyn)
	{
	case XYN_JOG:
		injog = TRUE;
		break;
	case XYN_READY:
		if (injog)
		{
			injog = false;
			dev.getXYPos();
			dev.DevInfo.nLocatePoints = 1;
			dev.DevInfo.LocatePoints = &dev.xypos;
			dev.Notify(VISN_LOCATE);
		}
		break;
	case XYN_ERROR:
		if (injog)
		{
			injog = false;
			dev.DevInfo.ErrorText = "XY Error!";
			dev.Notify(VISN_ERROR);
		}
	}

}


// VISCancel cancels any mode device is in
extern "C" _declspec (dllexport) void VISCancel(void)
{
	closeJog();
	dev.SrClientSocket_Loff();
	dev.SrClientSocket_Quit_Test();
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



