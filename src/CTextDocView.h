
#pragma once
#include "CDocView.h"
#include "CTextDocument.h"

#define PRINT_SIZE          5

#define INITIALISE          1000 // USED FOR DRAWING ENTIRE VIEW
#define TEXT                1001 // USED FOR TEXT EDITS - NEW TEXT, BACKSPACE ON TEXT
#define RETURN_BACK         1002 // USED FOR ADDING OR REMOVING LINES WITH BACKSPACE OR RETURN
#define EDITOR_EDIT         1003 // USED FOR ADDING OR REMOVING EDITORS
#define HIGHLIGHTING        1004 // USED FOR WHEN HIGHLIGHTING TEXT
#define SELECT_LINE         1007 // USED WHEN A DIFFERENT LINE IS SELECTED
#define REMOVE_HIGHLIGHT    1008 // USED WHEN A HIGHLIGHT IS BEING DELETED
#define CARET_MOVE          1009 // USED WHEN THE CARET IS MOVED WITH THE ARROW KEYS
#define ADD_RGN_SIDEBAR_BTN 1010 // USED TO ADD THE AREA OF THE SIDEBAR BUTTON TO THE REGION
#define ADD_RGN_ACTIVE_LINE 1011
#define H_SCROLL            1012
#define PASTE               1013


class CTextDocView :
    public CScrollView, public CDocView
{
protected:
    // Public constructors
    CTextDocView() noexcept;
    virtual ~CTextDocView();

    DECLARE_DYNCREATE(CTextDocView)

    // Public attributes
    CTextDocument* GetDocument() const;
    CSize GetDocSize();
    int getActiveEditor();

    // Public implementations
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void refresh(BOOL caret = TRUE, BOOL window = TRUE);

protected:

    // Protected implementations
    virtual void updateWindow(BOOL caret = TRUE, BOOL window = TRUE);
    void ResyncScrollSizes(BOOL docSize = TRUE, BOOL reposition = FALSE);

    // Protected overrides
    virtual void OnDraw(CDC* pDc);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDc, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDc, CPrintInfo* pInfo);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = (CPrintInfo*)0);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

    // Protected message handlers
    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    virtual void OnInitialUpdate();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    afx_msg void OnFilePrintPreview();

    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

    virtual void OnContextMenu(CWnd* pWnd, CPoint point);

    void OnCopy();
    void OnPaste();
    void OnCut();

    DECLARE_MESSAGE_MAP()

private:

    // Private Methods
    void expandLineBounds();
    void selectClippingRgn(int nAction, int type = -1);

    void drawHeader(CDC* pDc);

    // Private Resources
    int activeEditor;

    int recentEditor; // Used to store the last editor that was used to change lines
    int recentLine; // Used to store the last line that was used, to be used in combination with 'recentEditor'
    int recentBlockLine;
    BOOL recentHlght;
    int recentPrintZoom;

    CRgn* rgn;
    CPrintInfo* pInfo;
    BOOL printing;
    int printIterator;
    int returnNewLines;
};

#ifndef _DEBUG  // debug version in DesignArkView.cpp
inline CTextDocument* CTextDocView::GetDocument() const
{
    return reinterpret_cast<CTextDocument*>(m_pDocument);
}
#endif