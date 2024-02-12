/***************************************************************************
	NAME : CamCal.cpp
	DESC : Camera Calibaration Object
***************************************************************************/
#include "stdafx.h"

CAMCAL::CAMCAL()
{
	hWnd = 0;
	glgo = true;
	yellowpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 0));
	bluepen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	darkbluepen = CreatePen(PS_SOLID, 0, RGB(0, 0, 128));
	///=======================================
	///fake model
	///=======================================
	///#error make the hack ok valid false
	///#error option for fail on !calOk at prep or loc
	///#error option to override
		///HACK
	int j = 200;
	int k = 50;
	npCam = 4;
	XV2S cen(320, 240);
	pCam[0] = cen + XV2S(-j + k, -j);
	pCam[1] = cen + XV2S(j, -j + k);
	pCam[2] = cen + XV2S(j - k, j);
	pCam[3] = cen + XV2S(-j, j - k);
	rCam = V2(320, 240);
	computesolution();
	calOk = true;
	///REAL
	///calOk=false;
	width = 640;
	height = 480;
	///=======================================
}

CAMCAL::~CAMCAL()
{
	DeleteObject(yellowpen);
	DeleteObject(bluepen);
	DeleteObject(darkbluepen);
}

static BOOL GetPrivateProfileShortVect(char* sect, char* key, short* v, int n, char* ini)
{
	int iv[30];
	BOOL ok = GetPrivateProfileIntVect(sect, key, iv, n, ini);
	if (ok)
		for (int i = 0; i < n; i++)
			v[i] = (short)iv[i];
	return ok;
}

static void WritePrivateProfileVect(char* sect, char* key, double* v, int n, char* ini)
{
	///zzz
	char buf[512];
	int l = 0;
	for (int i = 0; i < n; i++)
		l += sprintf_s(buf + l, sizeof(buf) - l, l ? ",%0.6f" : "%0.6f", v[i]);
	WritePrivateProfileString(sect, key, buf, ini);
}
static void WritePrivateProfileShortVect(char* sect, char* key, short* v, int n, char* ini)
{
	///zzz
	char buf[512];
	int l = 0;
	for (int i = 0; i < n; i++)
		l += wsprintf(buf + l, l ? ",%d" : "%d", v[i]);
	WritePrivateProfileString(sect, key, buf, ini);
}


BOOL CAMCAL::loadsolution()
{
	XM3 _mx;
	XM3 _mxi;
	XV2S _pCam[4];
	V2 _rCam;

	BOOL mxOk = GetPrivateProfileVect(IniSection, (LPSTR)"mx", &_mx.m[0][0], 9, LibIni);
	BOOL mxiOk = GetPrivateProfileVect(IniSection, (LPSTR)"mxi", &_mxi.m[0][0], 9, LibIni);
	BOOL pCamOk = GetPrivateProfileShortVect(IniSection, (LPSTR)"pCam", &_pCam[0].x, 8, LibIni);
	BOOL rCamOk = GetPrivateProfileVect(IniSection, (LPSTR)"rCam", &_rCam.v2[0], 2, LibIni);
	if (mxOk && mxiOk && pCamOk && rCamOk)
	{
		mx = _mx;
		mxi = _mxi;
		for (int i = 0; i < 4; i++)
			pCam[i] = _pCam[i];
		rCam = _rCam;
		npCam = 4;
	}

	///	zzz call
	///	zzz code
	return false;
}

void CAMCAL::savesolution()
{
	WritePrivateProfileVect(IniSection, (LPSTR)"mx", &mx.m[0][0], 9, LibIni);
	WritePrivateProfileVect(IniSection, (LPSTR)"mxi", &mxi.m[0][0], 9, LibIni);
	WritePrivateProfileShortVect(IniSection, (LPSTR)"pCam", &pCam[0].x, 8, LibIni);
	WritePrivateProfileVect(IniSection, (LPSTR)"rCam", &rCam.v2[0], 2, LibIni);

	/*
	nonexistent WriteProfileVect(
	///	zzz call
	///	zzz code
	zzz ini
	zzz sect
	zzz key
	zzz use IMSLIB32 GetPrivateProvileVect(
	zzz =mx[0..2][0..2]
	*/
}

#define QORZEPSILON 0.000001 // a pixel is 1/640 so 1/1000000 should be enough
// returns a/b or if b is close to 0, 0
static double qorz(double a, double b)
{
	return ((b > QORZEPSILON) || (b < -QORZEPSILON)) ? a / b : 0.0;
}

void CAMCAL::computesolution()
{
	XV3 a(pCam[0]); // a,b,c,d = homiginized pCam[0..3]
	XV3 b(pCam[1]);
	XV3 c(pCam[2]);
	XV3 d(pCam[3]);

	XM3 mx1 = XM3Trans(-XV2(a));
	XM3 mx1i = XM3Trans(XV2(a));

	a = a * mx1;
	b = b * mx1;
	c = c * mx1;
	d = d * mx1;

	/// prevent /0
	///	double shy=-((b.y()-a.y())/(b.x()-a.x()));
	double shy = -qorz(b.y() - a.y(), b.x() - a.x());
	XM3 mx2 = XM3Shear(0, shy);
	XM3 mx2i = XM3Shear(0, -shy);
	a = a * mx2;
	b = b * mx2;
	c = c * mx2;
	d = d * mx2;

	/// prevent /0
	///	double shx=-((d.x()-a.x())/(d.y()-a.y()));
	double shx = -qorz(d.x() - a.x(), d.y() - a.y());
	XM3 mx3 = XM3Shear(shx, 0);
	XM3 mx3i = XM3Shear(-shx, 0);
	a = a * mx3;
	b = b * mx3;
	c = c * mx3;
	d = d * mx3;


	//m4=M3PerspX(f)
	double s = -qorz(c.y() - d.y(), c.x() - d.x());
	double f = qorz(-d.y(), s);
	XM3 mx4 = (f == 0.0) ? XM3() : XM3PerspX(f);
	XM3 mx4i = (f == 0.0) ? XM3() : XM3PerspX(-f);
	a = a * mx4;
	b = b * mx4;
	c = c * mx4;
	d = d * mx4;


	//m5=M3PerspY(f)
	s = -qorz(c.x() - b.x(), c.y() - b.y());
	f = qorz(-b.x(), s);
	XM3 mx5 = (f == 0.0) ? XM3() : XM3PerspY(f);
	XM3 mx5i = (f == 0.0) ? XM3() : XM3PerspY(-f);
	a = a * mx5;
	b = b * mx5;
	c = c * mx5;
	d = d * mx5;


	XM3 mx6 = XM3Scale(1.0 / c.x(), 1.0 / c.y());
	XM3 mx6i = XM3Scale(c.x(), c.y());


	//	solution matrix
	mx = mx1 * mx2 * mx3 * mx4 * mx5 * mx6;


	// inverse
	mxi = mx6i * mx5i * mx4i * mx3i * mx2i * mx1i;
}

V2 CAMCAL::cam2dev(V2& pcam)
{
	if (!calOk)
		return V2(0, 0);
	XV2 rdev(XV3(rCam) * mx);
	XV2 pdev(XV3(pcam) * mx);
	XV2 ddev(pdev - rdev);
	return V2(ddev.v[0], ddev.v[1]);
}

void CAMCAL::glPrep(HWND hwnd)
{
	hWnd = hwnd;
	RECT r;
	GetClientRect(hWnd, &r);
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL,    // support OpenGL 
		PFD_TYPE_RGBA,         // RGBA type 
		24,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer 
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0,0,0 };                // layer masks ignored 

	// get the best available match of pixel format for the device context  
	HDC hdc = GetDC(hWnd);
	int iPixelFormat = ChoosePixelFormat(hdc, &pfd);

	// make that the pixel format of the device context 
	SetPixelFormat(hdc, iPixelFormat, &pfd);

	hglrc = wglCreateContext(hdc);

	ReleaseDC(hWnd, hdc);
}

void CAMCAL::glDone()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	hglrc = NULL;
}

void CAMCAL::glDraw(HDC hdc)
{
	HPEN oldpen = (HPEN)SelectObject(hdc, GetStockObject(BLACK_PEN));

	if (glgo)
	{
		wglMakeCurrent(hdc, hglrc);
		glPushMatrix();
		glScaled(2.0 / width, -2.0 / height, 1.0);
		glTranslated(-width / 2.0, -height / 2.0, 0);
		glClearColor(0, 0, 0, 1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);

		glScissor(0, 0, width, height);
		glEnable(GL_SCISSOR_TEST);

		glLineWidth(1.5f);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_LINES);
	}

	for (int i = 0; i < npCam; i++)
	{
		int x = pCam[i].x;
		int y = pCam[i].y;
		dot(hdc, x, y);
	}
	displaysolution(hdc);
	displayaxes(hdc);

	if (glgo)
	{
		glEnd();
		glFlush();
		glPopMatrix();
		wglMakeCurrent(NULL, NULL);
	}

	SelectObject(hdc, oldpen);
}

void CAMCAL::displaysolution(HDC hdc)
{
	int ximin, ximax, yimin, yimax;
	XV2 v00(XV3(XV2S(0, 0)) * mx); // get view coordinates of image corners
	XV2 v10(XV3(XV2S(width, 0)) * mx);
	XV2 v11(XV3(XV2S(width, height)) * mx);
	XV2 v01(XV3(XV2S(0, height)) * mx);
	double xmin = v00.x();
	xmin = min(xmin, v01.x());
	xmin = min(xmin, v11.x());
	xmin = min(xmin, v10.x());
	double xmax = v00.x();
	xmax = max(xmax, v01.x());
	xmax = max(xmax, v11.x());
	xmax = max(xmax, v10.x());
	double ymin = v00.y();
	ymin = min(ymin, v01.y());
	ymin = min(ymin, v11.y());
	ymin = min(ymin, v10.y());
	double ymax = v00.y();
	ymax = max(ymax, v01.y());
	ymax = max(ymax, v11.y());
	ymax = max(ymax, v10.y());
	ximin = (int)floor(xmin * 10.0);
	ximax = (int)ceil(xmax * 10.0);
	yimin = (int)floor(ymin * 10.0);
	yimax = (int)ceil(ymax * 10.0);
	for (int xi = ximin; xi <= ximax; xi++)
	{
		double x = xi / 10.0;
		XV2 v0(XV3(x, 0.0, 1.0) * mxi);
		XV2 v1(XV3(x, 1.0, 1.0) * mxi);
		ExtendLineSeg(v0, v1);
		if (glgo)
		{
			glColor3d(0, 0, (xi % 10) ? 0.5 : 1.0);
			v0.glVertex();
			v1.glVertex();
		}
		else
		{
			SelectObject(hdc, (xi % 10) ? darkbluepen : bluepen);
			v0.MoveTo(hdc);
			v1.LineTo(hdc);
		}
	}

	for (int yi = yimin; yi <= yimax; yi++)
	{
		double y = yi / 10.0;
		XV2 v0(XV3(0.0, y, 1.0) * mxi);
		XV2 v1(XV3(1.0, y, 1.0) * mxi);
		ExtendLineSeg(v0, v1);
		if (glgo)
		{
			glColor3d(0, 0, (yi % 10) ? 0.5 : 1.0);
			v0.glVertex();
			v1.glVertex();
		}
		else
		{
			SelectObject(hdc, (yi % 10) ? darkbluepen : bluepen);
			v0.MoveTo(hdc);
			v1.LineTo(hdc);
		}
	}
}


void CAMCAL::displayaxes(HDC hdc)
{
	if (glgo)
		glColor3d(1, 1, 0);
	else
		SelectObject(hdc, yellowpen);

	XV2 xv0(XV3(0.0, 0.0, 1.0) * mxi); // y axis
	XV2 xv1(XV3(0.0, 1.0, 1.0) * mxi);
	XV2 yv0(XV3(0.0, 0.0, 1.0) * mxi); // x axes
	XV2 yv1(XV3(1.0, 0.0, 1.0) * mxi);
	ExtendLineSeg(xv0, xv1);
	ExtendLineSeg(yv0, yv1);
	if (glgo)
	{
		xv0.glVertex();
		xv1.glVertex();
		yv0.glVertex();
		yv1.glVertex();
	}
	else
	{
		xv0.MoveTo(hdc);
		xv1.LineTo(hdc);
		yv0.MoveTo(hdc);
		yv1.LineTo(hdc);
	}
}

void CAMCAL::dot(HDC hdc, int x, int y)
{
	const int i = 3;
	if (glgo)
	{
		XV2S a;
		XV2S b;
		glColor3d(1, 0, 0);

		a.x = x;  a.y = y - i;
		b.x = x + i; b.y = y;
		a.glVertex();
		b.glVertex();

		a = b;
		b.x = x;  b.y = y + i;
		a.glVertex();
		b.glVertex();

		a = b;
		b.x = x - i;	b.y = y;
		a.glVertex();
		b.glVertex();

		a = b;
		b.x = x; b.y = y - i;
		a.glVertex();
		b.glVertex();

	}
	else
		Ellipse(hdc, x - i, y - i + 1, x + i, y + i + 1);
}

void CAMCAL::ExtendLineSeg(XV2& v0, XV2& v1)
{
	XV2 v = v1 - v0;
	// compute line equation ax+by=c
	double a = -v.y();
	double b = v.x();
	double c = a * v0.x() + b * v0.y();
	if (a != 0.0 || b != 0.0)
	{
		if (fabs(b) < fabs(a)) // vertical[ish]
		{
			double y0 = (a < 0) ? height : 0.0;
			double y1 = (a < 0) ? 0.0 : height;
			v0.v[0] = -(b / a) * y0 + (c / a);
			v0.v[1] = y0;
			v1.v[0] = -(b / a) * y1 + (c / a);
			v1.v[1] = y1;
		}
		else // horizontal[ish]
		{
			double x0 = (b < 0) ? width : 0.0;
			double x1 = (b < 0) ? 0.0 : width;
			v0.v[0] = x0;
			v0.v[1] = -(a / b) * x0 + (c / b);
			v1.v[0] = x1;
			v1.v[1] = -(a / b) * x1 + (c / b);
		}
	}
}
