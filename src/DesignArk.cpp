
// DesignArk.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "DesignArk.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "CTextDocument.h"
#include "CTextDocView.h"

#include "CAboutDlg.h"
#include "CSmartColour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDesignArkApp

BEGIN_MESSAGE_MAP(CDesignArkApp, CWinAppEx)
	ON_COMMAND(ID_VIEW_SMARTCOLOURWINDOW, &CDesignArkApp::OnSmartColour)
	ON_COMMAND(ID_HELP_OPENWEBSITE, &CDesignArkApp::OnOpenSite)
	ON_COMMAND(ID_HELP_VIEWUSERDOCSONLINE, &CDesignArkApp::OnUserdocs)
	ON_COMMAND(ID_APP_ABOUT, &CDesignArkApp::OnAppAbout)

	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CDesignArkApp construction
CDesignArkApp::CDesignArkApp() noexcept
{
	m_bHiColorIcons = TRUE;
	m_nAppLook = 0;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	SetAppID(_T("DesignArk.Verison.1.0"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	this->smartColour_String.resize(5);
	this->smartColour_Colour.resize(5);

	this->smartColour_String[0] = std::vector<CString>{
		L"ELIF",
		L"ELSE",
		L"FOR",
		L"IF",
		L"IN",
		L"RETURN",
		L"WHILE"
	};
	this->smartColour_Colour[0] = COLORREF(RGB(128, 0, 128));

	this->smartColour_String[1] = std::vector<CString>{ 
		L"AS",
		L"CREATE", 
		L"DECLARE", 
		L"DISPLAY", 
		L"DO", 
		L"FUNCTION", 
		L"INITIALISE", 
		L"INITIALLY", 
		L"PRINT", 
		L"PROCEDURE", 
		L"SET", 
		L"TO",
	};
	this->smartColour_Colour[1] = COLORREF(RGB(128, 0, 32));

	this->smartColour_String[2] = std::vector<CString>{ 
		L"ARRAY", 
		L"BOOLEAN",
		L"CLASS", 
		L"CONST", 
		L"DICTIONARY", 
		L"FLOAT", 
		L"INT", 
		L"LIST", 
		L"OVERLOAD"
		L"REAL",
		L"STRING",
		L"VIRTUAL",
		L"VOID"
	};
	this->smartColour_Colour[2] = COLORREF(RGB(0, 0, 255));

	this->smartColour_String[3] = std::vector<CString>{
		L"vector"
	};
	this->smartColour_Colour[3] = COLORREF(RGB(0, 255, 0));

	this->smartColour_String[4] = std::vector<CString>{
		L"RANDOM"
	};
	this->smartColour_Colour[4] = COLORREF(RGB(255, 0, 0));

	this->commentType = L"//";
	this->commentColour = COLORREF(RGB(255, 0, 0));

	this->numberColour = COLORREF(RGB(0, 255, 0));
	this->highlightColour = COLORREF(RGB(110, 192, 250));

	this->zoom = 1.f;
	this->sFont = L"Courier New";
	this->indentSize = 4;

	// Initilise the GDI stuff
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
CDesignArkApp::~CDesignArkApp()
{
}

// The one and only CDesignArkApp object
CDesignArkApp theApp;

// CDesignArkApp initialisation
BOOL CDesignArkApp::InitInstance()
{
	/*////////////////////////////////////////
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin); // Console can be used be debugging
	////////////////////////////////////////*/

	// Initialise the common controls
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction();

	AfxInitRichEdit2(); // is required to use RichEdit control

	// Register app with OS and load INI file options (including MRU)
	SetRegistryKey(_T("DesignArk"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Initialise managers
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Initialise document template
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DesignArkTYPE,
		RUNTIME_CLASS(CTextDocument),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTextDocView));
	if (!pDocTemplate)
		return FALSE;
	this->AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it

	//CRect client;
	//pMainFrame->GetWindowRect(client);
	//pMainFrame->MoveWindow(client.left, client.top, 750, 900);

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}
int CDesignArkApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// Public Commands
void CDesignArkApp::OnSmartColour()
{
	CString tempIndentSize;
	tempIndentSize.Format(L"%d", this->indentSize);
	CSmartColour smartColour(this->smartColour_String, this->smartColour_Colour, this->commentType, this->commentColour, this->numberColour, this->highlightColour, this->sFont, tempIndentSize);

	if (smartColour.DoModal() == IDOK) {

		this->smartColour_String[0] = ((CSmartColourStatements*)smartColour.GetPage(0))->m_Strings_Statements_syntax_var;
		this->smartColour_Colour[0] = ((CSmartColourStatements*)smartColour.GetPage(0))->m_Colour_Statements_syntax_var;

		this->smartColour_String[1] = ((CSmartColourStatements*)smartColour.GetPage(0))->m_Strings_Statements_statements_var;
		this->smartColour_Colour[1] = ((CSmartColourStatements*)smartColour.GetPage(0))->m_Colour_Statements_statements_var;

		this->smartColour_String[2] = ((CSmartColourTypes*)smartColour.GetPage(1))->m_listbox_types_strings;
		this->smartColour_Colour[2] = ((CSmartColourTypes*)smartColour.GetPage(1))->m_Combo_Types_colour_var;

		this->smartColour_String[3] = ((CSmartColourBuiltin*)smartColour.GetPage(2))->m_Listbox_Builtin_Classes_var;
		this->smartColour_Colour[3] = ((CSmartColourBuiltin*)smartColour.GetPage(2))->m_Combo_Builtin_Classes_colour_var;

		this->smartColour_String[4] = ((CSmartColourBuiltin*)smartColour.GetPage(2))->m_Listbox_Builtin_Functions_var;
		this->smartColour_Colour[4] = ((CSmartColourBuiltin*)smartColour.GetPage(2))->m_Combo_Builtin_Functions_colour_var;

		this->commentType = ((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_comments_type_var;
		this->commentColour = ((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_comments_colour_var;

		this->numberColour = ((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_numbers_colour_var;
		
		this->highlightColour = ((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_highlight_colour_var;

		this->sFont = ((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_fonts_var;

		this->indentSize = _wtoi(((CSmartColourOther*)smartColour.GetPage(3))->m_pCombo_indent_var);

		// Will need to fixed if more views/docs are implemeted but will do for v1.0
		if (this->m_ActiveView) {
			((CTextDocView*)this->m_ActiveView)->refresh();
		}
	}
}

void CDesignArkApp::OnAppAbout() // This bit is not working
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal(); // This line is triggering a debug assertion failure
}
void CDesignArkApp::OnOpenSite()
{
	ShellExecute(0, 0, L"http://localhost/DesignArk/", 0, 0, SW_SHOW);
}
void CDesignArkApp::OnUserdocs()
{
	ShellExecute(0, L"open", L"http://localhost/DesignArk/userdocs.php", 0, 0, SW_SHOW);
}

// Protected Implementation
void CDesignArkApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}
void CDesignArkApp::LoadCustomState()
{
}
void CDesignArkApp::SaveCustomState()
{
}

