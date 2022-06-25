// CSmartColourOps.cpp : implementation file
//

#include "pch.h"
#include "DesignArk.h"
#include "afxdialogex.h"
#include "CSmartColourStatements.h"


// CSmartColourOps dialog

IMPLEMENT_DYNAMIC(CSmartColourStatements, CMFCPropertyPage)

CSmartColourStatements::CSmartColourStatements(std::vector<CString> syntax_string, COLORREF syntax_colour, std::vector<CString> statement_string, COLORREF statement_colour, CWnd* pParent/* = nullptr*/)
	: CMFCPropertyPage(IDD_SMARTCOLOUR_OPS /*, pParent*/)
	, m_Listbox_Statements_syntax_control()
	, m_Listbox_Statements_statments_control()
{
	this->m_Strings_Statements_syntax_var = syntax_string;
	this->m_Colour_Statements_syntax_var = syntax_colour;

	this->m_Strings_Statements_statements_var = statement_string;
	this->m_Colour_Statements_statements_var = statement_colour;
}

CSmartColourStatements::~CSmartColourStatements()
{
}

void CSmartColourStatements::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCVSLISTBOX_OPS, m_Listbox_Statements_statments_control);
	DDX_Control(pDX, IDC_COMBO_COLOUR_OPS, m_Combo_Statements_statments_control);
	DDX_Control(pDX, IDC_MFCVSLISTBOX_OPS2, m_Listbox_Statements_syntax_control);
	DDX_Control(pDX, IDC_COMBO_COLOUR_OPS2, m_Combo_Statements_syntax_control);
}


BOOL CSmartColourStatements::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (auto& it : this->m_Strings_Statements_syntax_var) {
		this->m_Listbox_Statements_syntax_control.AddItem(it);
	}

	this->m_Combo_Statements_syntax_control.SetColor(this->m_Colour_Statements_syntax_var);

	for (auto& it : this->m_Strings_Statements_statements_var) {
		this->m_Listbox_Statements_statments_control.AddItem(it);
	}

	this->m_Combo_Statements_statments_control.SetColor(this->m_Colour_Statements_statements_var);

	return 0;
}

BOOL CSmartColourStatements::OnKillActive()
{
	// Syntax
	this->m_Strings_Statements_syntax_var.resize(this->m_Listbox_Statements_syntax_control.GetCount());

	for (int i = 0; i < this->m_Listbox_Statements_syntax_control.GetCount(); i++) {
		this->m_Strings_Statements_syntax_var[i] = this->m_Listbox_Statements_syntax_control.GetItemText(i);
	}

	// Insertion Sort
	for (int i = 1; i < this->m_Strings_Statements_syntax_var.size(); i++) {
		CString currentValue = this->m_Strings_Statements_syntax_var[i];
		int pos = i;

		while (pos > 0 && this->m_Strings_Statements_syntax_var[pos - 1] > currentValue) {
			this->m_Strings_Statements_syntax_var[pos] = this->m_Strings_Statements_syntax_var[pos - 1];
			pos--;
		}
		this->m_Strings_Statements_syntax_var[pos] = currentValue;
	}

	this->m_Colour_Statements_syntax_var = this->m_Combo_Statements_syntax_control.GetColor();

	// Statements
	this->m_Strings_Statements_statements_var.resize(this->m_Listbox_Statements_statments_control.GetCount());

	for (int i = 0; i < this->m_Listbox_Statements_statments_control.GetCount(); i++) {
		this->m_Strings_Statements_statements_var[i] = this->m_Listbox_Statements_statments_control.GetItemText(i);
	}

	// Insertion Sort
	for (int i = 1; i < this->m_Strings_Statements_statements_var.size(); i++) {
		CString currentValue = this->m_Strings_Statements_statements_var[i];
		int pos = i;

		while (pos > 0 && this->m_Strings_Statements_statements_var[pos - 1] > currentValue) {
			this->m_Strings_Statements_statements_var[pos] = this->m_Strings_Statements_statements_var[pos - 1];
			pos--;
		}
		this->m_Strings_Statements_statements_var[pos] = currentValue;
	}

	this->m_Colour_Statements_statements_var = this->m_Combo_Statements_statments_control.GetColor();

	return CMFCPropertyPage::OnKillActive();
}



BEGIN_MESSAGE_MAP(CSmartColourStatements, CMFCPropertyPage)
END_MESSAGE_MAP()


// CSmartColourOps message handlers
