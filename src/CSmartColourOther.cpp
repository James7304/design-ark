// CSmartColourOther.cpp : implementation file
//

#include "pch.h"
#include "DesignArk.h"
#include "afxdialogex.h"
#include "CSmartColourOther.h"


// CSmartColourOther dialog

IMPLEMENT_DYNAMIC(CSmartColourOther, CMFCPropertyPage)

CSmartColourOther::CSmartColourOther(CString comments_type, COLORREF comments_colour, COLORREF numbers_colour, COLORREF highlight_colour, CString fonts, CString indentSize, CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_SMARTCOLOUR_OTHER/*, pParent*/)
{
	this->m_pCombo_comments_type_var = comments_type;
	this->m_pCombo_comments_colour_var = comments_colour;
	this->m_pCombo_numbers_colour_var = numbers_colour;
	this->m_pCombo_highlight_colour_var = highlight_colour;
	this->m_pCombo_fonts_var = fonts;
	this->m_pCombo_indent_var = indentSize;
}

CSmartColourOther::~CSmartColourOther()
{
}

void CSmartColourOther::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SMARTCOLOUR_OTHERS_COMMENT_COMBO, m_pCombo_comments_type_control);
	DDX_Control(pDX, IDC_SMARTCOLOUR_OTHER_COMMENTS_COLOUR, m_pCombo_comments_colour_control);
	DDX_Control(pDX, IDC_SMARTCOLOUR_OTHERS_NUMBERS_COLOUR, m_pCombo_numbers_colour_control);
	DDX_Control(pDX, IDC_SMARTCOLOUR_OTHERS_NUMBERS_COLOUR2, m_pCombo_highlight_colour_control);
	DDX_Control(pDX, IDC_SMARTCOLOUR_OTHERS_FONT, m_pCombo_fonts_control);
	DDX_Control(pDX, IDC_SMARTCOLOR_OTHER_INDENT, m_pCombo_indent_control);
}
BOOL CSmartColourOther::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->m_pCombo_comments_type_control.SetCurSel(this->m_pCombo_comments_type_control.FindString(-1, this->m_pCombo_comments_type_var));
	this->m_pCombo_comments_colour_control.SetColor(this->m_pCombo_comments_colour_var);

	this->m_pCombo_numbers_colour_control.SetColor(this->m_pCombo_numbers_colour_var);
	this->m_pCombo_highlight_colour_control.SetColor(this->m_pCombo_highlight_colour_var);

	this->m_pCombo_fonts_control.SetCurSel(this->m_pCombo_fonts_control.FindString(-1, this->m_pCombo_fonts_var));
	this->m_pCombo_indent_control.SetWindowTextW(this->m_pCombo_indent_var);

	return 0;
}
BOOL CSmartColourOther::OnKillActive()
{
	this->m_pCombo_comments_type_control.GetLBText(this->m_pCombo_comments_type_control.GetCurSel(), this->m_pCombo_comments_type_var);
	this->m_pCombo_comments_colour_var = this->m_pCombo_comments_colour_control.GetColor();

	this->m_pCombo_numbers_colour_var = this->m_pCombo_numbers_colour_control.GetColor();
	this->m_pCombo_highlight_colour_var = this->m_pCombo_highlight_colour_control.GetColor();

	this->m_pCombo_fonts_control.GetLBText(this->m_pCombo_fonts_control.GetCurSel(), this->m_pCombo_fonts_var);
	this->m_pCombo_indent_control.GetWindowTextW(this->m_pCombo_indent_var);

	return CMFCPropertyPage::OnKillActive();
}


BEGIN_MESSAGE_MAP(CSmartColourOther, CMFCPropertyPage)
END_MESSAGE_MAP()