
#pragma once

class CTextHandler
{
public:
	// Public constructors
	CTextHandler();
	~CTextHandler();

	// Public commands
	CString RecieveText();
	void DestroyText();

	// Public message handlers
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	// Private resources
	CString text;
	BOOL shft;
};

