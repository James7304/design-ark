
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CMDIFrameWndEx
{
public:
    // Public Constructors
    CMainFrame() noexcept;
    virtual ~CMainFrame();

    //Public Overrides
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

    // Public Implementations
    DECLARE_DYNAMIC(CMainFrame)
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    // Protected Implementations
    DECLARE_MESSAGE_MAP()

    // Protected Resources
    CMFCMenuBar       m_wndMenuBar;
    CMFCToolBar       m_wndToolBar;
    CMFCStatusBar     m_wndStatusBar;
    CMFCToolBarImages m_UserImages;

    // Protected Message Handlers
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowManager();
    afx_msg void OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    virtual void OnSetActiveView(CView* pViewNew, BOOL bNotify = TRUE);

private:
    // Private Initialisers
    int init_tabs();
    int init_menubar();
    int init_toolbar();
    int init_statusbar();
    int init_dockingwindows();
    int init_customisation();
    int init_userimages();
    int init_commands();
};
