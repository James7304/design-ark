#pragma once
#include "afxdialogex.h"


// CSmartColourBuiltin dialog

class CSmartColourBuiltin : public CMFCPropertyPage
{

public:
	CSmartColourBuiltin(std::vector<CString> classes_string, COLORREF classes_colour, std::vector<CString> functions_string, COLORREF functions_colour, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSmartColourBuiltin();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMARTCOLOUR_BUILTIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

	DECLARE_DYNAMIC(CSmartColourBuiltin)
	DECLARE_MESSAGE_MAP()
public:

	// Controls
	CVSListBox m_Listbox_Builtin_Classes_control;
	CMFCColorButton m_Combo_Builtin_Classes_colour_control;

	CVSListBox m_Listbox_Builtin_Functions_control;
	CMFCColorButton m_Combo_Builtin_Functions_colour_control;

	// Variables
	std::vector<CString> m_Listbox_Builtin_Classes_var;
	COLORREF m_Combo_Builtin_Classes_colour_var;

	std::vector<CString> m_Listbox_Builtin_Functions_var;
	COLORREF m_Combo_Builtin_Functions_colour_var;
};
