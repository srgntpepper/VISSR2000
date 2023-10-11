// SrEthernetSample.cpp : Defines the class behaviors for the application.
//

#include "stdAfx.h"
#include "SrEthernetSample.h"
#include "SrEthernetSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSrEthernetSampleApp

BEGIN_MESSAGE_MAP(CSrEthernetSampleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSrEthernetSampleApp construction

CSrEthernetSampleApp::CSrEthernetSampleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSrEthernetSampleApp object

CSrEthernetSampleApp theApp;


// CSrEthernetSampleApp initialization

BOOL CSrEthernetSampleApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CSrEthernetSampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
