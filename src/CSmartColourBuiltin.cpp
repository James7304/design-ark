// CSmartColourBuiltin.cpp : implementation file
//

#include "pch.h"
#include "DesignArk.h"
#include "afxdialogex.h"
#include "CSmartColourBuiltin.h"


// CSmartColourBuiltin dialog

IMPLEMENT_DYNAMIC(CSmartColourBuiltin, CMFCPropertyPage)

CSmartColourBuiltin::CSmartColourBuiltin(std::vector<CString> classes_string, COLORREF classes_colour, std::vector<CString> functions_string, COLORREF functions_colour, CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_SMARTCOLOUR_BUILTIN/*, pParent*/)
{
	this->m_Listbox_Builtin_Classes_var = classes_string;
	this->m_Combo_Builtin_Classes_colour_var = classes_colour;

	this->m_Listbox_Builtin_Functions_var = functions_string;
	this->m_Combo_Builtin_Functions_colour_var = functions_colour;
}

CSmartColourBuiltin::~CSmartColourBuiltin()
{
}

void CSmartColourBuiltin::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCVSLISTBOX_BUILTIN_CLASSES, m_Listbox_Builtin_Classes_control);
	DDX_Control(pDX, IDC_COMBO_COLOUR_BUILTIN_CLASSES, m_Combo_Builtin_Classes_colour_control);
	DDX_Control(pDX, IDC_MFCVSLISTBOX_BUILTIN_FUNCTIONS, m_Listbox_Builtin_Functions_control);
	DDX_Control(pDX, IDC_COMBO_COLOUR_BUILTIN_FUNCTIONS, m_Combo_Builtin_Functions_colour_control);
}

BOOL CSmartColourBuiltin::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (auto& it : this->m_Listbox_Builtin_Classes_var) {
		this->m_Listbox_Builtin_Classes_control.AddItem(it);
	}
	this->m_Combo_Builtin_Classes_colour_control.SetColor(this->m_Combo_Builtin_Classes_colour_var);

	for (auto& it : this->m_Listbox_Builtin_Functions_var) {
		this->m_Listbox_Builtin_Functions_control.AddItem(it);
	}
	this->m_Combo_Builtin_Functions_colour_control.SetColor(this->m_Combo_Builtin_Functions_colour_var);

	return 0;
}

BOOL CSmartColourBuiltin::OnKillActive()
{
	// Classes
	this->m_Listbox_Builtin_Classes_var.resize(this->m_Listbox_Builtin_Classes_control.GetCount());

	for (int i = 0; i < this->m_Listbox_Builtin_Classes_control.GetCount(); i++) {
		this->m_Listbox_Builtin_Classes_var[i] = this->m_Listbox_Builtin_Classes_control.GetItemText(i);
	}

	// Insertion Sort
	for (int i = 1; i < this->m_Listbox_Builtin_Classes_var.size(); i++) {
		CString currentValue = this->m_Listbox_Builtin_Classes_var[i];
		int pos = i;

		while (pos > 0 && this->m_Listbox_Builtin_Classes_var[pos - 1] > currentValue) {
			this->m_Listbox_Builtin_Classes_var[pos] = this->m_Listbox_Builtin_Classes_var[pos - 1];
			pos--;
		}
		this->m_Listbox_Builtin_Classes_var[pos] = currentValue;
	}

	this->m_Combo_Builtin_Classes_colour_var = this->m_Combo_Builtin_Classes_colour_control.GetColor();

	//Functions
	this->m_Listbox_Builtin_Functions_var.resize(this->m_Listbox_Builtin_Functions_control.GetCount());

	for (int i = 0; i < this->m_Listbox_Builtin_Functions_control.GetCount(); i++) {
		this->m_Listbox_Builtin_Functions_var[i] = this->m_Listbox_Builtin_Functions_control.GetItemText(i);
	}

	// Insertion Sort
	for (int i = 1; i < this->m_Listbox_Builtin_Functions_var.size(); i++) {
		CString currentValue = this->m_Listbox_Builtin_Functions_var[i];
		int pos = i;

		while (pos > 0 && this->m_Listbox_Builtin_Functions_var[pos - 1] > currentValue) {
			this->m_Listbox_Builtin_Functions_var[pos] = this->m_Listbox_Builtin_Functions_var[pos - 1];
			pos--;
		}
		this->m_Listbox_Builtin_Functions_var[pos] = currentValue;
	}

	this->m_Combo_Builtin_Functions_colour_var = this->m_Combo_Builtin_Functions_colour_control.GetColor();


	return CMFCPropertyPage::OnKillActive();
}


BEGIN_MESSAGE_MAP(CSmartColourBuiltin, CMFCPropertyPage)
END_MESSAGE_MAP()


// CSmartColourBuiltin message handlers
