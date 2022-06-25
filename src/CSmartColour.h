#pragma once

#include "CSmartColourStatements.h"
#include "CSmartColourTypes.h"
#include "CSmartColourBuiltin.h"
#include "CSmartColourOther.h"

// CSmartColour

class CSmartColour : public CPropertySheet
{

public:
	CSmartColour(std::vector<std::vector<CString>> page_strings, std::vector<COLORREF> page_colour, 
					CString commentType, COLORREF commentColour, COLORREF numberColour, COLORREF highlightColour, CString fonts, CString indents, 
					LPCTSTR pszCaption = L"", CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~CSmartColour();

protected:

	CSmartColourTypes types;
	CSmartColourStatements statements;
	CSmartColourBuiltin builtin;
	CSmartColourOther other;

	DECLARE_DYNAMIC(CSmartColour)
	DECLARE_MESSAGE_MAP()
};


