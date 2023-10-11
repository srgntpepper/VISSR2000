// SrEthernetSample.h : main header file for the VISSR200032 application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSrEthernetSampleApp:
// See SrEthernetSample.cpp for the implementation of this class
//

class CSrEthernetSampleApp : public CWinApp
{
public:
	CSrEthernetSampleApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSrEthernetSampleApp theApp;