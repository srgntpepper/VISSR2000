
/*******************************************************************************
	Name : CamCal.h
	Desc : Camera Calibration Module
	Revision : $Revision$
*******************************************************************************/
///Do below marked with /// | #error |zzz
class CAMCAL
{
	HWND hWnd;
	int width;
	int height;
	HGLRC hglrc;
	BOOL glgo; // GL option

public:
	XV2S pCam[4];  // 4 camera points collected for calibration device rect
	int npCam;     // number of points collected so far
	V2 rCam; // Ref postition collected with 4 pCam
private:
	XM3 mx;         // solution matrix
	XM3 mxi;        // mx inverse

public:
	CAMCAL();
	~CAMCAL();
	BOOL loadsolution();
	void savesolution();
	void computesolution();
	V2 cam2dev(V2& pcam);
	BOOL calOk;


	///============================================
	///#error do this stuff
	/*============================================
	[ ]do (elsewhere)

	open loop prep option to go to jog and always pass regardless of response
		this will be used to jog and cal the locator origin

	fix missing xCal on debug display on notify

	fix where ims cal post update clobbers delta

	prompt before accepting calibration
		then save

	void ...:savesolution(char* profile)
		IF NULL USE [default] ELSE USE [profile]
		save original xy locations and pixel positions
		save ref position too (wee need it)
	BOOL ...:loadsolution(char* profile)
		IF NULL USE [default] ELSE USE [profile]
	---
	on constructor
		calOk=
	---
	on prep
		determine profile (or NULL)
		calOk=loadsolution(profile)
	on locate
		if !calOk ERROR
		?does this get in our way?
	---
	prep parser
		add support for spaces
		add -r profile (for pRofile)
		change -c to -cal
	============================================*/
	void glPrep(HWND hwnd);
	void glDone();
	void glDraw(HDC hdc);
private:
	HPEN yellowpen;
	HPEN bluepen;
	HPEN darkbluepen;

	void displaysolution(HDC hdc);
	void displayaxes(HDC hdc);
	void dot(HDC hdc, int x, int y);
	void ExtendLineSeg(XV2& v0, XV2& v1);
};
