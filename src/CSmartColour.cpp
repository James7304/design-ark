// CSmartColour.cpp : implementation file
//

#include "pch.h"
#include "DesignArk.h"

#include "CSmartColour.h"

// CSmartColour

IMPLEMENT_DYNAMIC(CSmartColour, CPropertySheet)

CSmartColour::CSmartColour(std::vector<std::vector<CString>> page_strings, std::vector<COLORREF> page_colour, 
							CString commentType, COLORREF commentColour, COLORREF numberColour, COLORREF highlightColour, CString fonts, CString indents, 
							LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage),
	statements(page_strings[0], page_colour[0], page_strings[1], page_colour[1]),
	types(page_strings[2], page_colour[2]),
	builtin(page_strings[3], page_colour[3], page_strings[4], page_colour[4]),
	other(commentType, commentColour, numberColour, highlightColour, fonts, indents)
{
	this->AddPage(&this->statements);
	this->AddPage(&this->types);
	this->AddPage(&this->builtin);
	this->AddPage(&this->other);
}

CSmartColour::~CSmartColour()
{
}


BEGIN_MESSAGE_MAP(CSmartColour, CPropertySheet)
END_MESSAGE_MAP()


// CSmartColour message handlers
