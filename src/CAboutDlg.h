#pragma once
#include "afxdialogex.h"


// CAboutDlg dialog

class CAboutDlg : public CDialogEx
{

public:
	CAboutDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APP_ABOUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_DYNAMIC(CAboutDlg)
	DECLARE_MESSAGE_MAP()
};
