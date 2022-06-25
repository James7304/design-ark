
// DesignArk.h : main header file for the DesignArk application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

/*///////

To find the number of lines in project, run

(gci -include *.h,*.cpp -recurse | select-string .).Count

in the code folder

(last count: 5298)

///////*/


// CDesignArkApp:
// See DesignArk.cpp for the implementation of this class
//

class CDesignArkApp : public CWinAppEx
{
public:
	// Public Constructors
	CDesignArkApp() noexcept;
	~CDesignArkApp();

	// Public Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Public Commands
	afx_msg void OnSmartColour();

	afx_msg void OnAppAbout();
	afx_msg void OnOpenSite();
	afx_msg void OnUserdocs();

	// Public Implementations
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	CView* m_ActiveView;

	// Public Resources
	// SmartColour Preferences
	std::vector<std::vector<CString>> smartColour_String;
	std::vector<COLORREF> smartColour_Colour;

	CString commentType;
	COLORREF commentColour;
	COLORREF numberColour;
	COLORREF highlightColour;

	// App work preferences
	float zoom;
	CString sFont;
	int indentSize;

protected:
	// Protected Implementation
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	DECLARE_MESSAGE_MAP()
};

extern CDesignArkApp theApp;
