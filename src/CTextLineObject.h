#pragma once

#include "CAppObject.h"

class CTextLineObject
	: public CAppObject
{
public:
	// Public constructors
	CTextLineObject(CRect bounds, CString ID, std::vector<int> lineNums, CString text = L"", BOOL active = FALSE);
	~CTextLineObject();

	//DECLARE_SERIAL(CTextLineObject)

	// Public commands
	int draw(CDC* pDc, CSize textExtent, std::vector<std::vector<int>> brackets, int returnNewLines, BOOL printing, int printAreaLength = -1);

	// Public getters & setters
	CString getText();
	int getLength();
	void setText(CString text);
	void concatenateString(CString text, int position);
	void backspace(int position);

	BOOL isHighlighting();
	void setHighlighting(BOOL val);
	void setHighlighter(int pos1, int pos2);
	void highlightLine();
	BOOL isLineHighlighted();
	int getHStart();
	int getHEnd();
	CString getHighlightedText();

	int getNumSubLines();
	std::vector<int> iGetLineNums();
	CString sGetLineNums();
	void addSublines(std::vector<int> subs);
	void incrementLine(int subline, int val);

	virtual void setBounds(CRect bounds);
	virtual void move(int x, int y);

private:

	// Private Functions
	BOOL onlyNums(CString str);
	BOOL brkPosContains(std::vector<std::vector<int>> vector, int value, int side = 1);

	// Private resources
	CString text;

	Gdiplus::Rect highlighter;
	BOOL highlight;
	int hStart;
	int hEnd;
	BOOL lineHighlight;

	int numSubLines;
	std::vector<int> lineNums;

	BOOL smartColour;
};

