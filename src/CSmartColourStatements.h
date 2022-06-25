#pragma once
#include "afxdialogex.h"


// CSmartColourOps dialog

class CSmartColourStatements : public CMFCPropertyPage
{

public:
	CSmartColourStatements(std::vector<CString> syntax_string, COLORREF syntax_colour, std::vector<CString> statement_string, COLORREF statement_colour, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSmartColourStatements();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMARTCOLOUR_OPS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSmartColourStatements)
public:
	// Controls
	CVSListBox m_Listbox_Statements_syntax_control;
	CMFCColorButton m_Combo_Statements_syntax_control;

	CVSListBox m_Listbox_Statements_statments_control;
	CMFCColorButton m_Combo_Statements_statments_control;

	// Variables
	std::vector<CString> m_Strings_Statements_syntax_var;
	COLORREF m_Colour_Statements_syntax_var;

	std::vector<CString> m_Strings_Statements_statements_var;
	COLORREF m_Colour_Statements_statements_var;

};
