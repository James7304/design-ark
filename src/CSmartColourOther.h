#pragma once
#include "afxdialogex.h"


// CSmartColourOther dialog

class CSmartColourOther : public CMFCPropertyPage
{

public:
	CSmartColourOther(CString comments_type, COLORREF comments_colour, COLORREF numbers_colour, COLORREF highlight_colour, CString fonts, CString indentSize, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSmartColourOther();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMARTCOLOUR_OTHER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

	DECLARE_DYNAMIC(CSmartColourOther)
	DECLARE_MESSAGE_MAP()
public:

	// Controls
	CComboBox m_pCombo_comments_type_control;
	CMFCColorButton m_pCombo_comments_colour_control;
	CMFCColorButton m_pCombo_numbers_colour_control;
	CMFCColorButton m_pCombo_highlight_colour_control;
	CComboBox m_pCombo_fonts_control;
	CEdit m_pCombo_indent_control;

	// Variables
	CString m_pCombo_comments_type_var;
	COLORREF m_pCombo_comments_colour_var;
	COLORREF m_pCombo_numbers_colour_var;
	COLORREF m_pCombo_highlight_colour_var;
	CString m_pCombo_fonts_var;
	CString m_pCombo_indent_var;
};
