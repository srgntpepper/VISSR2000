#pragma once
#include "stdAfx.h"
class SR2000DEV
{
	BOOL isOpen;

	//Device info
	char DeviceName[64];
	char DeviceIP[16];
	char CommandPort[6];
	char DataPort[6];
	char materialID[24];
	char batchID[24];
	char DeviceProgId[16];
	char DeviceAppFileName[128];
	char VisObjectDir[128];

	//timeout variable set by ini - SettlingTime
	int timeoutSeconds;

	char* pDeviceName;	// used to switch between Device Names
	char* pDeviceAppFileName;	// Used to switch aplications in the scanner
	char* pVisObjectDir;

	//Used by winsocket2
	BOOL socketsLoaded;
	WSADATA wsaData;
	BOOL connected;
	SOCKET s_commandSocket;	// used to switch between Device Sockets
	SOCKET s_dataSocket;


	char ebuf[128]; // computed error message buffer
	char matchCode[64];///xxx
	char progid[5];
	BOOL PowerUpOk;
	HANDLE hLiveLocateThread;
	DWORD dwLiveLocateThreadId;
	BOOL killLive;
	BOOL init(WORD idc, HWND parent, LPSTR ini, LPSTR section);
	BOOL locateFailed;
	int camx;
	int camy;

	///seq
public:
	int calIndex;   // index of next point to store
private:
	P3 calXYPos[5]; // cal xy positions : center and 4 corner positions
	//V2 calCamPos[5]; // cal camera pixel positions : ref and 4 corners
	//V2 calXYOffset; //  vector from ini with real space vector to move cam to put item in upper left

public:

	VISINFO DevInfo; // The devices VISINFO
	BOOL optJog; // set by -j in prep
	BOOL optNoJog; // set by -n in prep
	BOOL optCal; // set by -c in prep
	WORD idc;
	HWND parent;
	char emsg[100]; // error message or NULL
	HWND xywnd;
	P3 xypos;  // xy table position of locate
	P3 locpos; // xy table position of located pixel (vs xy as trained)
	//CAMCAL camcal;  // The Cameral Calibration

	enum vismode { xRead, xInspect, xLocate, xLiveLocate, xLocatePostJog, xSnap, xCal } mode;

	SR2000DEV();
	~SR2000DEV();
	//BOOL powerup(char* ini, char* section);
	BOOL open(WORD idc, HWND parent, LPSTR ini, LPSTR section);
	void prep(char* prepstring);
	//BOOL syncProgTrigRead(enum vismode xmode);
	void Notify(WORD n);

	BOOL startLiveLocate();
	BOOL killLiveLocate();
	BOOL isLiveLocateRunning() { return (BOOL)hLiveLocateThread; }
	void getXYPos();
	//void moveXYRel(V2& v);

	//Enter device specific routines, in this case for SR2000...
	void SrClientSocket_Init();
	void SrClientSocket_Connect();
	void SrClientSocket_Disconnect();
	void SrClientSocket_Lon();
	void SrClientSocket_Loff();
	void SrClientSocket_Test();
	void SrClientSocket_Quit_Test();
	void SrClientSocket_Compare(const std::string& data);
	void SrClientSocket_Receive(std::string code, std::string prepstring);
	void SrClientSocket_Inspect();
	void SrClientSocket_Read();

	//Helper Functions
	std::string socketCommunication();
	void handleTimeout();
	void handleError(const char* message);
	void showMessageBox(const char* message);


private:
	//BOOL selProg(SOCKET cliSock);
	//BOOL trigAndParse(SOCKET cliSock);
	DWORD LiveLocateProc();
	friend DWORD WINAPI _LiveLocateProc(LPSTR lpData);

};
