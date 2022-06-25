
#pragma once
#include "DesignArk.h"

class CAppObject
{
public:
	// Public constructors
	CAppObject(CRect bounds, CString ID, BOOL active = FALSE);
	~CAppObject();

	// Public commands
	virtual void draw(CDC* pDc);

	// Public message handers
	virtual void OnSize(UINT nType, int cx, int cy);

	virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnMouseMove(UINT nFlags, CPoint point);

	virtual int OnRecieveText(CString text);
	virtual void OnRecieveBackspace();
	virtual void OnRecieveReturn();
	virtual void OnRecieveTab();

	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL OnContextMenu(CWnd* pWnd, CPoint point);

	// Public getters, setters and checkers
	CString getID();
	void setID(CString ID);

	CRect getBounds();
	BOOL checkPointInBounds(CPoint point);

	virtual void setBounds(CRect bounds);
	virtual void setBounds(int x, int y, int cx, int cy);
	virtual void setPosition(CPoint point);
	virtual void move(int x, int y);

	BOOL isActive();
	virtual void setActive(BOOL val);

protected:
	CString ID;
	CRect bounds;
	BOOL active;
};

