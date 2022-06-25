#pragma once
#include "afxdialogex.h"


// CSmartColourTypes dialog

class CSmartColourTypes : public CMFCPropertyPage
{

public:
	CSmartColourTypes(std::vector<CString> types_string, COLORREF colour, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSmartColourTypes();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMARTCOLOUR_TYPES };
#endif#
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();

	DECLARE_DYNAMIC(CSmartColourTypes)
	DECLARE_MESSAGE_MAP()

public:

	// Controls
	CVSListBox m_Listbox_Types_control;
	CMFCColorButton m_Combo_Types_colour_control;

	// Variables
	COLORREF m_Combo_Types_colour_var;
	std::vector<CString> m_listbox_types_strings;
};
