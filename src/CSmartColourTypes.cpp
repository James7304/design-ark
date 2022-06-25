// CSmartColourTypes.cpp : implementation file
//

#include "pch.h"
#include "DesignArk.h"
#include "afxdialogex.h"
#include "CSmartColourTypes.h"


// CSmartColourTypes dialog

IMPLEMENT_DYNAMIC(CSmartColourTypes, CMFCPropertyPage)

CSmartColourTypes::CSmartColourTypes(std::vector<CString> types_string, COLORREF colour, CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_SMARTCOLOUR_TYPES/*, pParent */)
	, m_Listbox_Types_control()
{
	this->m_listbox_types_strings = types_string;
	this->m_Combo_Types_colour_var = colour;
}
CSmartColourTypes::~CSmartColourTypes()
{
}

void CSmartColourTypes::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MFCVSLISTBOX_TYPES, m_Listbox_Types_control);
	DDX_Control(pDX, IDC_COMBO_COLOUR_TYPES, m_Combo_Types_colour_control);
}

BOOL CSmartColourTypes::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (auto& it : this->m_listbox_types_strings) {
		this->m_Listbox_Types_control.AddItem(it);
	}
	this->m_Combo_Types_colour_control.SetColor(this->m_Combo_Types_colour_var);

	return 0;
}

BOOL CSmartColourTypes::OnKillActive()
{
	this->m_listbox_types_strings.resize(this->m_Listbox_Types_control.GetCount());

	for (int i = 0; i < this->m_Listbox_Types_control.GetCount(); i++) {
		this->m_listbox_types_strings[i] = this->m_Listbox_Types_control.GetItemText(i);
	}

	// Insertion Sort
	for (int i = 1; i < this->m_listbox_types_strings.size(); i++) {
		
		CString currentValue = this->m_listbox_types_strings[i];
		int pos = i;

		while (pos > 0 && this->m_listbox_types_strings[pos - 1] > currentValue) {
			this->m_listbox_types_strings[pos] = this->m_listbox_types_strings[pos - 1];
			pos--;
		}
		this->m_listbox_types_strings[pos] = currentValue;
	}

	this->m_Combo_Types_colour_var = this->m_Combo_Types_colour_control.GetColor();

	return CMFCPropertyPage::OnKillActive();
}


BEGIN_MESSAGE_MAP(CSmartColourTypes, CMFCPropertyPage)
END_MESSAGE_MAP()
