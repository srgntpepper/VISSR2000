// SrEthernetSampleDlg.h : header file
//

#pragma once


// CSrEthernetSampleDlg dialog
class CSrEthernetSampleDlg : public CDialog
{
// Construction
public:
	CSrEthernetSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SRETHERNETSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedLon();
	afx_msg void OnBnClickedLoff();
	afx_msg void OnBnClickedReceive();
};
