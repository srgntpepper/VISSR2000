// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


#if !defined(AFX_STDAFX_H__0322ED96_9E13_4DFA_AA5A_F765423FF4CC__INCLUDED_)
#define AFX_STDAFX_H__0322ED96_9E13_4DFA_AA5A_F765423FF4CC__INCLUDED_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _AFXDLL  // Add this line to enable MFC DLL usage

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*		Running into issues with this commented code conflicting with shared vs static dll libraries,
*		commented out to ensure static or /MT DLL
*/
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>            // MFC socket extensions

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <winsock2.h>
#include <cstdlib>
#include <string>
#include <io.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>
#include <math.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <codecvt>
#include <ShlObj.h>

#include <imslib32.h>
#include <im5000.h>
#include <xy32.h>
#include <vis32.h>
#include <jvmath32.H>

#include "xvmath.h"
//#include "CamCal.h"
#include "vissr200032.h"


//#include "framework.h"
//#include "SrClientSocketApi.h"

#include "resource.h"
#endif // !defined(AFX_STDAFX_H__0322ED96_9E13_4DFA_AA5A_F765423FF4CC__INCLUDED_)
