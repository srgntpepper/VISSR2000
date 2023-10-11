// SrEthernetSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SrEthernetSample.h"
#include "SrEthernetSampleDlg.h"
#include "SrClientSocketApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSrEthernetSampleDlg dialog




CSrEthernetSampleDlg::CSrEthernetSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSrEthernetSampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSrEthernetSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSrEthernetSampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DISCONNECT, &CSrEthernetSampleDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_CONNECT, &CSrEthernetSampleDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_LON, &CSrEthernetSampleDlg::OnBnClickedLon)
	ON_BN_CLICKED(IDC_LOFF, &CSrEthernetSampleDlg::OnBnClickedLoff)
	ON_BN_CLICKED(IDC_RECEIVE, &CSrEthernetSampleDlg::OnBnClickedReceive)
END_MESSAGE_MAP()


// CSrEthernetSampleDlg message handlers

BOOL CSrEthernetSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SrClinetSocket_Init();
	CEdit* pEdit1 = (CEdit*)GetDlgItem(IDC_COMMAND);
	CEdit* pEdit2 = (CEdit*)GetDlgItem(IDC_DATA);
	pEdit1->SetWindowTextW(_T("9004"));
	pEdit2->SetWindowTextW(_T("9004"));
	pEdit1->UpdateWindow();
	pEdit2->UpdateWindow();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSrEthernetSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSrEthernetSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSrEthernetSampleDlg::OnBnClickedConnect()
{
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC);
	pStatic->SetWindowTextW(_T("Connecting..."));
	pStatic->UpdateWindow();
	
	CString str1,str2;


	CEdit* pEdit1 = (CEdit*)GetDlgItem(IDC_COMMAND);
	CEdit* pEdit2 = (CEdit*)GetDlgItem(IDC_DATA);
	pEdit1->GetWindowTextW(str1);
	pEdit2->GetWindowTextW(str2);

	SrClientSocket_Connect(_ttoi(str1),_ttoi(str2));

	pStatic->SetWindowTextW(_T(""));
	pStatic->UpdateWindow();
}

void CSrEthernetSampleDlg::OnBnClickedDisconnect()
{
	SrClientSocket_Disconnect();
}

void CSrEthernetSampleDlg::OnBnClickedLon()
{
	SrClientSocket_Lon();
}

void CSrEthernetSampleDlg::OnBnClickedLoff()
{
	SrClientSocket_Loff();
}

void CSrEthernetSampleDlg::OnBnClickedReceive()
{
	SrClientSocket_Receive();
}
