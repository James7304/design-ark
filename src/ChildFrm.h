
// ChildFrm.h : interface of the CChildFrame class
//

#pragma once

class CChildFrame : public CMDIChildWndEx
{
public:
	// Implementation
	CChildFrame() noexcept;
	virtual ~CChildFrame();
	DECLARE_DYNCREATE(CChildFrame)

	// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	// Resources
	CSplitterWndEx m_wndSplitter;

	// Generated message map functions
	DECLARE_MESSAGE_MAP()
};
