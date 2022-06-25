
// CTextEditorObject.cpp : implementation of the CTextEditorObject class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DesignArk.h"
#endif

#include "CTextEditorObject.h"

// Public contructors
CTextEditorObject::CTextEditorObject(CRect bounds, CString ID, 
										BOOL lineNums, int maxLines, int defBoxHeight, 
											BOOL active,
												std::vector<CString> text, std::vector<int> line, int lineOffset)
	: CAppObject(bounds, ID, active)
{
	this->activeLine = 1;
	this->mouseLine = 0;
	this->blockLine = 0;
	this->printLine = 0;
	this->caretPos = text[text.size() - 1].GetLength();
	this->maxLines = maxLines;

	this->defBoxHeight = defBoxHeight;

	this->hlght = FALSE;
	this->hlghting = FALSE;
	this->hlghtStartP.x = 0; this->hlghtStartP.y = 0;
	this->hlghtEndP.x = 0; this->hlghtEndP.y = 0;

	this->lineNums = lineNums;
	this->lineOffset = lineOffset;
	this->hoverSubLine = FALSE;
	this->clickSubLine = FALSE;

	int i = 1;
	CString id;
	CRect lineBounds = bounds;
	std::vector<int> lineNumbers = line;

	if (this->lineNums) {

		lineBounds.left += (this->lineOffset * theApp.zoom);
	}

	lineBounds.bottom = lineBounds.top + static_cast<int>(this->defBoxHeight * theApp.zoom);

	for (auto& text : text) {

		id.Format(L"%d", i-1);

		this->lines.push_back(new CTextLineObject(lineBounds, id, lineNumbers, text, FALSE));
		lineBounds.top += static_cast<int>(this->defBoxHeight * theApp.zoom);
		lineBounds.bottom += static_cast<int>(this->defBoxHeight * theApp.zoom);

		lineNumbers[lineNumbers.size()-1]++;
		i++;
	}

	this->lines[0]->setActive(TRUE);

	this->cursor_arrow = 0;
	this->lMouseDown = FALSE;


	this->setSidebar();
	this->setBracketsNull();
}
CTextEditorObject::~CTextEditorObject()
{
	for (auto& it : this->lines) {
		delete it;
	}
}

// Public implementations
int CTextEditorObject::draw(CDC* pDc, CSize textExtent, int xScrollPosition, int returnNewLines, BOOL printing, CRect printAreaLength)
{
	this->textExtent = pDc->GetTextExtent(L"A", 1);

	if (printing) {
		this->setBracketsNull();
	}

	if (this->lineNums) {

		if (!printing) {
			for (auto& it : this->lines) {
				returnNewLines = it->draw(pDc, this->textExtent, this->brackets, returnNewLines, printing);
			}
		}

		// Prints the sidebar
		if (!printing) {
			this->sidebar.left = xScrollPosition;
			this->sidebar.right = xScrollPosition + static_cast<int>(this->defBoxHeight * theApp.zoom);
		}
		else {
			this->sidebar.left = 0;
			this->sidebar.right = 0;
		}

		if (!printing) {

			CPen penWhite;
			penWhite.CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
			CBrush brushWhite;
			brushWhite.CreateSolidBrush(RGB(255, 255, 255));

			CPen* pOldPen = pDc->SelectObject(&penWhite);
			CBrush* oldBrush = pDc->SelectObject(&brushWhite);

			pDc->Rectangle(CRect(this->sidebar.left, this->sidebar.top + (returnNewLines * this->defBoxHeight * theApp.zoom), this->sidebar.left + (this->lineOffset * theApp.zoom), this->sidebar.bottom + 2));

			pDc->SelectObject(pOldPen);
			pDc->SelectObject(oldBrush);

			pDc->RoundRect(this->sidebar, CPoint(8 * theApp.zoom, 8 * theApp.zoom));

			pDc->SetTextColor(RGB(50, 50, 150));

			if (this->blockLine != 0) {

				Gdiplus::Graphics g(pDc->GetSafeHdc());

				if (this->lMouseDown) {
					Gdiplus::Bitmap bitmap(L"res/arrow_right_click.bmp");

					Gdiplus::Rect expansionRect(this->sidebar.left + static_cast<int>(this->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetWidth() * theApp.zoom) / 2, this->lines[this->blockLine - 1]->getBounds().top + static_cast<int>(this->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetHeight() * theApp.zoom) / 2,
					(bitmap.GetWidth() * theApp.zoom), (bitmap.GetHeight() * theApp.zoom));

					g.DrawImage(&bitmap, expansionRect);
				}
				else {
					Gdiplus::Bitmap bitmap(L"res/arrow_right.bmp"); 

					Gdiplus::Rect expansionRect(this->sidebar.left + static_cast<int>(this->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetWidth() * theApp.zoom) /2, this->lines[this->blockLine - 1]->getBounds().top + static_cast<int>(this->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetHeight() * theApp.zoom) /2,
														(bitmap.GetWidth() * theApp.zoom), (bitmap.GetHeight() * theApp.zoom));

					g.DrawImage(&bitmap, expansionRect);
				}
			}
		}

		if (printing) {

			int i = this->printLine;
			BOOL finished = FALSE;

			while (!finished && i < this->lines.size()) {

				int lines = std::ceil(float(this->lines[i]->getText().GetLength()) / float((printAreaLength.Width() - this->lines[i]->getBounds().left * 2) / this->textExtent.cx));
				if (lines == 0) {
					lines++;
				}

				if (this->lines[i]->getBounds().top + (lines + returnNewLines) * this->getBoxHeight() < printAreaLength.bottom) {
					
					pDc->SetTextColor(RGB(50, 50, 150));
					pDc->TextOut(this->sidebar.left + 400 - (this->lines[i]->sGetLineNums().GetLength() * this->textExtent.cx), this->lines[i]->getBounds().top + this->lines[i]->getBounds().Height() / 2 - this->textExtent.cy / 2 + (returnNewLines * this->defBoxHeight * theApp.zoom), this->lines[i]->sGetLineNums(), this->lines[i]->sGetLineNums().GetLength());

					pDc->SetTextColor(RGB(0, 0, 0));
					returnNewLines = this->lines[i]->draw(pDc, this->textExtent, this->brackets, returnNewLines, printing, printAreaLength.Width());

					i++;
				}
				else {
					finished = TRUE;
				}

			}

			this->printLine = i;

			if (this->printLine == this->lines.size()) {
				this->printLine = 0;
			}
		}
		else {
			for (auto& it : this->lines) {

				pDc->TextOut(this->sidebar.left + (this->lineOffset * theApp.zoom) - it->sGetLineNums().GetLength() * this->textExtent.cx - 2, it->getBounds().top + it->getBounds().Height() / 2 - this->textExtent.cy / 2, it->sGetLineNums(), it->sGetLineNums().GetLength());
			}
		}
		pDc->SetTextColor(RGB(0, 0, 0));
	}
	else {
		for (int i = this->printLine; i < this->lines.size(); i++) {
			returnNewLines = this->lines[i]->draw(pDc, this->textExtent, this->brackets, returnNewLines, printing, printAreaLength.Width());
		}
	}

	return returnNewLines;
}

// Public getters and checkers
int CTextEditorObject::getCaretPos()
{
	return this->caretPos;
}

BOOL CTextEditorObject::pointHighlighted(CPoint point)
{
	// TODO : Check if point is highlighted
	return FALSE;
}
BOOL CTextEditorObject::hasHighlight()
{
	return this->hlght;
}
void CTextEditorObject::hlghtingOff()
{
	this->hlghting = FALSE;
	this->hlght = FALSE;
	this->startLine = 0;
	this->startPos = 0;

	this->recentHlght = FALSE;

	if (this->active) {
		this->lines[this->activeLine - 1]->setActive(TRUE);
	}

	for (auto& it : this->lines) {
		it->setHighlighting(FALSE);
	}
}

CRect CTextEditorObject::getHighlightClippingRect()
{
	CRect rgn;

	if (this->hlghting) {

		if (this->recentLine == this->activeLine) {

			if (!this->recentHlght && this->hlghting) {
				rgn = this->lines[this->activeLine - 1]->getBounds();
			}
			else if (this->recentPos != this->caretPos) {

				rgn = this->lines[this->activeLine - 1]->getBounds();
				rgn.right = rgn.left + max(this->recentPos, this->caretPos) * this->textExtent.cx + (2 * theApp.zoom);
				rgn.left = rgn.left + min(this->recentPos, this->caretPos) * this->textExtent.cx - (2 * theApp.zoom);
			}
		}
		else {
			rgn = this->lines[min(this->recentLine, this->activeLine) - 1]->getBounds();
			rgn.bottom = this->lines[max(this->recentLine, this->activeLine) - 1]->getBounds().bottom;
		}
	}

	return rgn;
}

CRgn* CTextEditorObject::getHighlightExactRgn(int x_offset, int y_offset)
{
	CRgn* rgn = new CRgn();
	VERIFY(rgn->CreateRectRgn(0, 0, 0, 0));

	for (auto& it : this->lines) {

		if(it->getHStart() != it->getHEnd()){

			CRgn* rgn1 = new CRgn();
			int x1 = (it->getHStart() * this->textExtent.cx) + it->getBounds().left - (2 * theApp.zoom) - x_offset,
				y1 = it->getBounds().top - y_offset,
				x2 = (it->getHEnd() * this->textExtent.cx) + it->getBounds().left + (2 * theApp.zoom) - x_offset,
				y2 = it->getBounds().bottom - y_offset;

			ASSERT(rgn1->CreateRectRgn(x1, y1, x2, y2));

			int nCombineResult = rgn->CombineRgn(rgn, rgn1, RGN_OR);
			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);

		}
	}
	return rgn;
}

int CTextEditorObject::getStartLine()
{
	return this->startLine - 1;
}
BOOL CTextEditorObject::isHlghtMultiline()
{
	BOOL first = FALSE;

	for (auto& it : this->lines) {
		if (it->isHighlighting()) {
			if (!first) {
				first = TRUE;
			}
			else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

int CTextEditorObject::lineHighlight(int line)
{
	if (this->lines[line]->isHighlighting()) {
		if (this->lines[line]->isLineHighlighted()) {
			return 2;
		}
		return 1;
	}
	return 0;
}

CPoint CTextEditorObject::getCaretPoint(CSize caretSize)
{
	CPoint caretPoint;
	BOOL els = TRUE;

	int offset = 0;

	if (this->lines[this->activeLine - 1]->isHighlighting() && (this->lines[this->activeLine - 1]->getLength() == 0)) {
		offset = 1;
	}

	if (this->caretPos >= this->lines[this->activeLine - 1]->getLength() + offset) {
		caretPoint.x = (this->lines[this->activeLine - 1]->getLength() + offset) * (this->textExtent.cx) + (this->lineOffset * theApp.zoom) + 1;
	}
	else {
		caretPoint.x = (this->caretPos) * this->textExtent.cx + (this->lineOffset * theApp.zoom) + 1;
	}

	caretPoint.y = this->bounds.top + ((this->activeLine - 0.5) * static_cast<int>(this->defBoxHeight * theApp.zoom)) - caretSize.cy / 2 ;

	return caretPoint;
}

CSize CTextEditorObject::getTextExtent()
{
	return this->textExtent;
}

void CTextEditorObject::setTextExtent(CSize size)
{
	this->textExtent = size;
}

int CTextEditorObject::getRecentPos()
{
	return this->recentPos;
}

void CTextEditorObject::incrementSublines(int subline, int val)
{
	for (auto& it : this->lines) {
		it->incrementLine(subline, val);
	}
}

void CTextEditorObject::setActive(BOOL active)
{
	this->active = active;

	if (!this->active) {

		this->hoverSubLine = FALSE;
		this->clickSubLine = FALSE;
		this->blockLine = 0;

		this->hlghtingOff();

		this->lines[this->activeLine - 1]->setActive(FALSE);
	}
	else {
		if (this->blockLine != 0) {
			this->activeLine = this->blockLine;
		}
		this->lines[this->activeLine - 1]->setActive(TRUE);
		this->caretPos = 0;
	}
}

void CTextEditorObject::move(int x, int y)
{
	CAppObject::move(x, y);
	for (auto& it : this->lines) {
		it->move(x, y);
	}
	this->sidebar += CPoint(x, y);
}
void CTextEditorObject::setBounds(CRect bounds) 
{
	this->bounds = bounds;
	CRect newBounds = this->bounds;
	newBounds.left = newBounds.left + static_cast<int>(this->lineOffset * theApp.zoom);
	newBounds.bottom = newBounds.top + static_cast<int>(this->defBoxHeight * theApp.zoom);

	for (auto& it : this->lines) {

		it->setBounds(newBounds);
		newBounds.top += static_cast<int>(this->defBoxHeight * theApp.zoom);
		newBounds.bottom += static_cast<int>(this->defBoxHeight * theApp.zoom);
	}
}

std::tuple<CRect, int> CTextEditorObject::getPrintBounds(int returnNewLines, int printAreaLength)
{
	CRect printBounds = this->lines[0]->getBounds();
	int thisBoundsNewLines = returnNewLines;

	int printMaxCharLength = (printAreaLength - this->lines[0]->getBounds().left * 2) / this->textExtent.cx;

	for (int i = 0; i < this->lines.size(); i++) {
		thisBoundsNewLines += std::ceil(double(this->lines[i]->getLength() / printMaxCharLength));

		if (i == this->printLine) {
			printBounds.top += this->defBoxHeight * theApp.zoom * thisBoundsNewLines;
		}
	}

	printBounds.right = printBounds.left + printMaxCharLength * this->textExtent.cx;
	printBounds.bottom = printBounds.top + this->defBoxHeight * theApp.zoom * (thisBoundsNewLines - returnNewLines + this->lines.size());

	return std::tuple<CRect, int>(printBounds, thisBoundsNewLines);
}

int CTextEditorObject::getCursorArrow()
{
	return this->cursor_arrow;
}
void CTextEditorObject::initialise()
{
	this->caretPos = 0;
	this->lines[this->activeLine - 1]->setActive(FALSE);
	this->activeLine = 1;
	this->lines[this->activeLine - 1]->setActive(TRUE);
}

// Public message handlers
void CTextEditorObject::OnSize(UINT nType, int cx, int cy)
{
	this->bounds.bottom = this->bounds.top + static_cast<int>(defBoxHeight * theApp.zoom) * (this->lines.size());

	CRect lineBounds;
	lineBounds.left = static_cast<int>(this->lineOffset * theApp.zoom);
	lineBounds.right = this->bounds.right;
	lineBounds.top = this->bounds.top;
	lineBounds.bottom = lineBounds.top + static_cast<int>(this->defBoxHeight * theApp.zoom);

	for (auto& it : this->lines) {

		it->setBounds(lineBounds);

		lineBounds.top += static_cast<int>(this->defBoxHeight * theApp.zoom);
		lineBounds.bottom += static_cast<int>(this->defBoxHeight * theApp.zoom);
	}

	this->setSidebar();
}

BOOL CTextEditorObject::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL redraw = FALSE;

	this->lMouseDown = FALSE;

	if (this->hlghting) {
		this->hlghting = FALSE;

		if ((this->startLine == this->activeLine && this->startPos == this->caretPos) || 

			(this->startLine == this->activeLine && 
			this->startPos >= this->lines[this->activeLine - 1]->getLength() && 
			this->caretPos >= this->lines[this->activeLine - 1]->getLength())) {


			this->hlghtingOff();
			redraw = TRUE;
		}
		else {
			this->hlght = TRUE;
		}
	}
	
	if (point.x <= this->sidebar.right) {
		BOOL update = FALSE;
		int i = 1;

		while (!update && i <= this->lines.size()) {

			if (point.y >= this->lines[i - 1]->getBounds().top &&
				point.y < this->lines[i - 1]->getBounds().bottom) {

				this->clickSubLine = TRUE;

				this->caretPos = 0;
				update = TRUE;

			}
			i++;
		}

	}
	return redraw;
}
BOOL CTextEditorObject::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->hlghtingOff();
	BOOL sidebarbtnhold = FALSE;
	this->lMouseDown = TRUE;

	BOOL update = FALSE;
	int i = 1;

	// Iterate through all the lines
	while (!update && i <= this->lines.size()) {

		// The point is in the lines
		if (point.y >= this->lines[i - 1]->getBounds().top && point.y < this->lines[i - 1]->getBounds().bottom) {

			// If we have changed lines
			if (i != this->activeLine) {

				this->lines[this->activeLine - 1]->setActive(FALSE);
				this->activeLine = i;
				this->lines[i - 1]->setActive(TRUE);
			}

			// If we have selected part of the text
			if (this->lines[i - 1]->checkPointInBounds(point)) {

				if (point.x - static_cast<int>(this->defBoxHeight * theApp.zoom) - (this->lineOffset * theApp.zoom) >= this->lines[i - 1]->getLength() * this->textExtent.cx) {
					this->caretPos = this->lines[i - 1]->getLength();
				}
				else {
					this->caretPos = (point.x - (this->lineOffset * theApp.zoom)) / this->textExtent.cx;
				}
			}

			// If we have selected the side of the text editor
			else {

				// If we selected a button
				if (point.x < static_cast<int>(this->defBoxHeight * theApp.zoom)) {
					this->blockLine = i;
					sidebarbtnhold = TRUE;
				}

				// If we select the bit that isn't the sidebar
				else {
					this->hlghtingOff();
					this->hlght = TRUE;
					this->lines[this->activeLine - 1]->highlightLine();
				}
				this->caretPos = 0;
			}

			update = TRUE;
		}

		i++;
	}

	if (!update && point.x >= static_cast<int>(this->defBoxHeight * theApp.zoom) + (this->lineOffset * theApp.zoom)) {
		this->lines[this->activeLine - 1]->setActive(FALSE);
		this->activeLine = this->lines.size();
		this->lines[this->activeLine - 1]->setActive(TRUE);
		this->caretPos = this->lines[this->activeLine - 1]->getLength();
	}

	this->hlghtStartP = point;
	this->setBracketsNull();

	return sidebarbtnhold;
}
void CTextEditorObject::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int leftpos = this->caretPos - 1, rightpos = this->caretPos;
	BOOL leftstop = FALSE, rightstop = FALSE;

	CString lineText = this->lines[this->activeLine - 1]->getText();

	while (!(leftstop && rightstop) && !(leftpos == 0 && rightpos == lineText.GetLength())) {

		if (leftpos == 0) {
			leftstop = TRUE;
		}
		else if(lineText.Mid(leftpos, 1) == L" " && !leftstop) {
			leftpos++;
			leftstop = TRUE;
		}
		else if(!leftstop) {
			leftpos--;
		}

		if ((lineText.Mid(rightpos, 1) == L" " && !rightstop) || rightpos == lineText.GetLength()) {
			rightstop = TRUE;
		}
		else if(!rightstop) {
			rightpos++;
		}
	}

	this->lines[this->activeLine - 1]->setHighlighter(leftpos, rightpos);
	this->hlght = TRUE;

	this->caretPos = rightpos;
}
void CTextEditorObject::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CTextEditorObject::OnRButtonDown(UINT nFlags, CPoint point)
{

	if (!this->hlght) {

		BOOL update = FALSE;
		int i = 1;

		while (!update && i <= this->lines.size()) {

			if (this->lines[i - 1]->checkPointInBounds(point)) {

				this->lines[this->activeLine - 1]->setActive(FALSE);
				this->activeLine = i;
				this->lines[i - 1]->setActive(TRUE);

				if (point.x - static_cast<int>(this->defBoxHeight * theApp.zoom) - (this->lineOffset * theApp.zoom) >= this->lines[i - 1]->getLength() * this->textExtent.cx) {
					this->caretPos = this->lines[i - 1]->getLength();
				}
				else {
					this->caretPos = (point.x - (this->lineOffset * theApp.zoom)) / this->textExtent.cx;
				}
				update = TRUE;
			}

			i++;
		}

	}

	//if (!update && point.x >= static_cast<int>(this->defBoxHeight * theApp.zoom)) {
	//	this->lines[this->activeLine - 1]->setActive(FALSE);
	//	this->activeLine = this->lines.size();
	//	this->lines[this->activeLine - 1]->setActive(TRUE);
	//	this->caretPos = this->lines[this->activeLine - 1]->getLength();
	//}

	this->setBracketsNull();
}
BOOL CTextEditorObject::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL mUpdate = FALSE, bUpdate = FALSE;
	BOOL brk = FALSE;

	int i = 1;
	BOOL redraw = FALSE;

	while (!mUpdate && !brk && i <= this->lines.size()) {

		if (point.y >= this->lines[i - 1]->getBounds().top && point.y < this->lines[i - 1]->getBounds().bottom) {

			this->mouseLine = i;
			mUpdate = TRUE;

			if (this->lineNums) {

				if (!this->lines[i - 1]->checkPointInBounds(point)) {

					if (point.x <= this->sidebar.right) {

						if (this->blockLine != i) {

							redraw = TRUE;
							this->blockLine = i;
						}
						bUpdate = TRUE;
						this->cursor_arrow = 2;
					}
					else {
						this->cursor_arrow = 1;
					}
				}
				else {
					this->cursor_arrow = 0;
				}
			}

			brk = TRUE;
		}
		i++;
	}
	if (!mUpdate) {
		this->mouseLine = 0;
		this->cursor_arrow = 1;
	}
	if (!bUpdate) {
		if (this->blockLine != 0) {
			redraw = TRUE;
		}
		this->blockLine = 0;
	}

	if (nFlags == MK_LBUTTON) {

		if (this->checkPointInBounds(point)) {
			this->hlghtEndP = point;
			this->setHighlighter();
		}
		else if (point.y < this->bounds.top) {
			this->hlghtEndP = CPoint(this->lines[0]->getBounds().left + 1, this->lines[0]->getBounds().top + static_cast<int>(this->defBoxHeight * theApp.zoom) /2);
		}
		else {
			this->hlghtEndP = CPoint(this->lines[this->lines.size() - 1]->getLength()*this->textExtent.cx + 1, this->lines[this->lines.size() - 1]->getBounds().top + static_cast<int>(this->defBoxHeight * theApp.zoom) / 2);
		}
		redraw = TRUE;
	}
	else {
		this->hlghting = FALSE;
	}

	return redraw;
}

BOOL CTextEditorObject::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO : Allow restriccted choice in context menu when selecting point that isnt highlighted
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, pWnd, TRUE);
#endif
	return TRUE;
}

int CTextEditorObject::OnRecieveText(CString text, BOOL open)
{
	int itPos; // This is needed as it gets changed in this->brackContains
	int carridgeOccur = 0;

	if ((text == L")" && this->bracketContains(this->caretPos, itPos, 0, 1)) || 
		(text == L"}" && this->bracketContains(this->caretPos, itPos, 1, 1)) || 
		(text == L"]" && this->bracketContains(this->caretPos, itPos, 2, 1)) || 
		(text == L"\"" && this->bracketContains(this->caretPos, itPos, 3, 1))|| 
		(text == L"'" && this->bracketContains(this->caretPos, itPos, 4, 1))) {

		this->brackets.erase(this->brackets.begin() + itPos);
		this->brackets.shrink_to_fit();

		this->caretPos++;
	}
	
	else {
		int carridge = text.Find(L"\n");
		CString recursiveText;
		if (carridge != -1) {

			recursiveText = text.Mid(carridge + 1);
			text = text.Mid(0, carridge);
		}
		this->lines[this->activeLine - 1]->concatenateString(text, this->caretPos);

		if (text == L"(") {

			this->lines[this->activeLine - 1]->concatenateString(L")", this->caretPos + 1);
			this->moveBrackets(2, this->caretPos);

			this->brackets.push_back({ this->caretPos, this->caretPos + 1, 0 });
		}
		else if (text == L"{") {

			this->lines[this->activeLine - 1]->concatenateString(L"}", this->caretPos + 1);
			this->moveBrackets(2, this->caretPos);

			this->brackets.push_back({ this->caretPos, this->caretPos + 1 , 1 });
		}
		else if (text == L"[") {

			this->lines[this->activeLine - 1]->concatenateString(L"]", this->caretPos + 1);
			this->moveBrackets(2, this->caretPos);

			this->brackets.push_back({ this->caretPos, this->caretPos + 1 , 2 });
		}
		else if (text == L"\"") {

			this->lines[this->activeLine - 1]->concatenateString(L"\"", this->caretPos + 1);
			this->moveBrackets(2, this->caretPos);

			this->brackets.push_back({ this->caretPos, this->caretPos + 1 , 3 });
		}
		else if (text == L"'") {

			this->lines[this->activeLine - 1]->concatenateString(L"'", this->caretPos + 1);
			this->moveBrackets(2, this->caretPos);

			this->brackets.push_back({ this->caretPos, this->caretPos + 1 , 4 });
		}
		else {
			this->moveBrackets(text.GetLength(), this->caretPos);
		}
		this->caretPos += text.GetLength();

		if (carridge != -1) {

			this->OnRecieveReturn(open);
			carridgeOccur++;
			carridgeOccur += this->OnRecieveText(recursiveText);
		}
	}
	return carridgeOccur;
}
void CTextEditorObject::OnRecieveBackspace()
{
	if (!this->hlght && this->activeLine > 0 && this->caretPos != 0) { // Remove a character

		CString activeChar = this->lines[this->activeLine - 1]->getText().Mid(this->caretPos - 1, 1);
		int itPos;

		if (((activeChar == L"(" || activeChar == L"{" || activeChar == L"[" || activeChar == L"\"" || activeChar == L"'") && this->bracketContains(this->caretPos - 1, itPos, 5, 0))){
			this->lines[this->activeLine - 1]->backspace(this->brackets[itPos][1]);
			this->moveBrackets(-1, this->brackets[itPos][1]);

			this->brackets.erase(this->brackets.begin() + itPos);
			this->brackets.shrink_to_fit();
		}
		
		else {

			CString spaces = L"";

			if ((this->caretPos % theApp.indentSize) == 0) {
				spaces = L"    ";

				if (this->lines[this->activeLine - 1]->getText().Mid(this->caretPos - theApp.indentSize, theApp.indentSize) == spaces) {

					for (int j = 0; j < theApp.indentSize-1; j++) {

						this->lines[this->activeLine - 1]->backspace(this->caretPos);
						this->moveBrackets(-1, this->caretPos);
						this->caretPos--;
					}
				}
			}
			else {
				for (int i = 0; i < (this->caretPos % theApp.indentSize); i++) {
					spaces += L" ";
				}

				if (this->lines[this->activeLine - 1]->getText().Mid(this->caretPos - (this->caretPos % theApp.indentSize), (this->caretPos % theApp.indentSize)) == spaces) {

					int it = (this->caretPos % theApp.indentSize) - 1;

					for (int j = 0; j < it; j++) {

						this->lines[this->activeLine - 1]->backspace(this->caretPos);
						this->moveBrackets(-1, this->caretPos);
						this->caretPos--;
					}
				}
			}
		}
		this->lines[this->activeLine - 1]->backspace(this->caretPos);
		this->moveBrackets(-1, this->caretPos);
		this->caretPos--;
	}

	else if (!this->hlghting && !this->hlght && this->activeLine > 1 && this->caretPos == 0) { // Remove a line

		this->bounds.bottom -= static_cast<int>(this->defBoxHeight * theApp.zoom);

		this->caretPos = this->lines[this->activeLine - 2]->getLength();
		this->lines[this->activeLine - 2]->concatenateString(this->lines[this->activeLine - 1]->getText(), this->caretPos);

		for (int i = this->lines.size() - 1; i >= this->activeLine; i--) {
			this->lines[i]->move(0, -static_cast<int>(this->defBoxHeight * theApp.zoom));
			this->lines[i]->setID(this->lines[i - 1]->getID());
			this->lines[i]->incrementLine(this->lines[i]->iGetLineNums().size() - 1, -1);
		}

		this->lines.erase(this->lines.begin() + this->activeLine - 1);
		this->activeLine--;
		this->lines[this->activeLine - 1]->setActive(TRUE);

		this->setSidebar();
	}

	else if (this->hlght) { // Remove a highlight

		BOOL first = FALSE;
		int i = 0;
		BOOL end = FALSE;

		while (!end && i < this->lines.size()) { // Iterate through all the lines

			if (this->lines[i]->isHighlighting()) {  // If the line is highlighting

				if (!first) { // If this is the first line that we have found

					this->activeLine = i + 1; // This will become our active line
					this->caretPos = this->lines[i]->getHStart(); // Set caret at the end of the line
					this->lines[i]->backspace(this->caretPos); // Remove all that we have highlighted on this line
					this->lines[i]->setHighlighting(FALSE); // Tell it we are no longer highlighting

					first = TRUE; // Let the loop know that we have found the first highlighted line
				}
				else if(!this->lines[i]->isLineHighlighted() || i == this->lines.size() - 1) { // If the entire line is not highlighted

					this->lines[i]->backspace(this->caretPos);
					this->lines[this->activeLine - 1]->concatenateString(this->lines[i]->getText(), this->caretPos);

					for (int j = i; j >= this->activeLine; j--) {

						this->lines.erase(this->lines.begin() + j);
					}

					for (int k = this->lines.size() - 1; k >= this->activeLine; k--) {

						this->lines[k]->move(0, -static_cast<int>(this->defBoxHeight * theApp.zoom) * (i - this->activeLine + 1));
						this->lines[k]->setID(this->lines[k]->getID());
						this->lines[k]->incrementLine(this->lines[k]->iGetLineNums().size() - 1, -(i - this->activeLine + 1));
					}

					this->bounds.bottom -= static_cast<int>(this->defBoxHeight * theApp.zoom) * (i - this->activeLine + 1);

					end = TRUE;
				}
			}
			else if (first) {

				end = TRUE;
			}
			i++;
		}

		this->hlghtingOff();
		this->setSidebar();
	}
}
void CTextEditorObject::OnRecieveTab()
{
	int itPos;
	if (this->bracketContains(this->caretPos, itPos, 5, 1)) {

		this->brackets.erase(this->brackets.begin() + itPos);
		this->brackets.shrink_to_fit();

		this->caretPos++;
	}
	else {
		CString indent = L"";
		for (int i = 0; i < theApp.indentSize; i++) {
			indent.Append(L" ");
		}
		this->OnRecieveText(indent);
	}
}
void CTextEditorObject::OnRecieveReturn(BOOL open)
{
	if (this->maxLines > this->lines.size() || this->maxLines == 0) { // If there is space for another line
		
		this->bounds.bottom += static_cast<int>(this->defBoxHeight * theApp.zoom);
		CString newLineText = L"";
		int numTab = 0;

		if (!open) {
			BOOL cln = TRUE, checkEnd = TRUE, checkStart = TRUE;
			int cmtPos = -1, clnPos = -1;

			int frontIt = 0;

			for (int i = this->lines[this->activeLine - 1]->getText().GetLength() - 1; i >= 0; i--) {

				if ((this->lines[this->activeLine - 1]->getText().Mid(i, 1) != L":" && this->lines[this->activeLine - 1]->getText().Mid(i, 1) != L" "/* && i == 0 // This may need put back if bugs are found */) && checkEnd) {
					cln = FALSE;
				}
				else if (this->lines[this->activeLine - 1]->getText().Mid(i, 1) == L":" && cln) {
					clnPos = i;
					checkEnd = FALSE;
				}

				if ((this->lines[this->activeLine - 1]->getText().Mid(frontIt, 1) != L" " || frontIt == this->lines[this->activeLine - 1]->getText().GetLength() - 1) && checkStart) {

					numTab = (frontIt - (frontIt % theApp.indentSize)) / theApp.indentSize;
					checkStart = FALSE;
				}
				if (this->lines[this->activeLine - 1]->getText().Mid(frontIt, theApp.commentType.GetLength()) == theApp.commentType) {
					cmtPos = frontIt;
				}

				frontIt++;
			}

			if (frontIt == 0) {
				cln = FALSE;
			}

			if (cln && (cmtPos >= clnPos || cmtPos == -1)) {
				numTab++;
			}

			CString indent = L"";
			for (int i = 0; i < theApp.indentSize; i++) {
				indent.Append(L" ");
			}

			for (int i = 0; i < numTab; i++) {
				newLineText.Append(indent);
			}
		}
		newLineText.Append(this->lines[this->activeLine - 1]->getText().Mid(this->caretPos));

		auto itpos = this->lines.begin() + this->activeLine;

		auto newit = this->lines.insert(
			itpos,
			new CTextLineObject(
				this->lines[this->activeLine - 1]->getBounds(),
				L"0",
				this->lines[this->activeLine - 1]->iGetLineNums(),
				newLineText,
				TRUE)); // Add a new line


		this->lines[this->activeLine - 1]->setText(this->lines[this->activeLine - 1]->getText().Mid(0, this->caretPos));
		this->lines[this->activeLine - 1]->setActive(FALSE); // Set old line not active

		this->caretPos = 0 + numTab * theApp.indentSize;
		this->activeLine++; // Change position for editing

		CString id;

		for (int i = this->activeLine-1; i < this->lines.size(); i++) {

			id.Format(L"%d", i);
			this->lines[i]->move(0, static_cast<int>(this->defBoxHeight * theApp.zoom));
			this->lines[i]->setID(id);
			this->lines[i]->incrementLine(this->lines[i]->iGetLineNums().size()-1, 1);
		} // Move all lines and reset all ids
	}

	this->setSidebar();
	this->setBracketsNull();
}

void CTextEditorObject::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {

	case VK_LEFT:
		if (this->caretPos > 0) {
			this->caretPos--;

			int itPos;

			if (this->bracketContains(this->caretPos, itPos, 5, 0)) {

				this->setBracketsNull();
			}
		}
		else if (this->caretPos == 0 && this->activeLine > 1) {

			this->lines[this->activeLine - 1]->setActive(FALSE);
			this->activeLine--;
			this->lines[this->activeLine - 1]->setActive(TRUE);

			this->setBracketsNull();

			this->caretPos = this->lines[this->activeLine - 1]->getLength();
		}
		break;

	case VK_UP:
		if (this->activeLine > 1) {
			this->lines[this->activeLine - 1]->setActive(FALSE);
			this->activeLine--;
			this->lines[this->activeLine - 1]->setActive(TRUE);

			this->setBracketsNull();
		}
		break;

	case VK_RIGHT:
		if (this->caretPos < this->lines[this->activeLine - 1]->getLength()) {

			int itPos;

			if (this->bracketContains(this->caretPos, itPos, 5, 1)) {

				this->brackets.erase(this->brackets.begin() + itPos);
				this->brackets.shrink_to_fit();
			}

			this->caretPos++;
		}
		else if (this->caretPos == this->lines[this->activeLine - 1]->getLength() && this->activeLine != this->lines.size()) {

			this->lines[this->activeLine - 1]->setActive(FALSE);
			this->activeLine++;
			this->lines[this->activeLine - 1]->setActive(TRUE);
			
			this->caretPos = 0;
		}

		break;
		
	case VK_DOWN:
		if (this->activeLine < this->lines.size()) {

			this->lines[this->activeLine - 1]->setActive(FALSE);
			this->activeLine++;
			this->lines[this->activeLine - 1]->setActive(TRUE);

			this->setBracketsNull();
		}

		break;

	}
}

// Public Queries
std::vector <int> CTextEditorObject::iGetLineNum(int a)
{
	if (a != 0) {
		return this->lines[a-1]->iGetLineNums();
	}

	else if (this->clickSubLine) {
		this->clickSubLine = FALSE;
		return this->lines[this->activeLine-1]->iGetLineNums();
	}
	return std::vector <int> { 0 };
}
CString CTextEditorObject::sGetLineNum(int a)
{
	if (a != 0) {
		return this->lines[a - 1]->sGetLineNums();
	}

	else if (this->clickSubLine) {
		this->clickSubLine = FALSE;
		return this->lines[this->activeLine - 1]->sGetLineNums();
	}
	return L"0";
}

int CTextEditorObject::getActiveLine()
{
	return this->activeLine-1;
}
int CTextEditorObject::getBlockLine()
{
	return this->blockLine - 1;
}
int CTextEditorObject::getPrintLine()
{
	return this->printLine;
}
CString CTextEditorObject::getLineText(int line)
{
	if (line == 0) {
		return this->lines[this->activeLine - 1]->getText();
	}
	return this->lines[line - 1]->getText();
}
CRect CTextEditorObject::getLineBounds(int line)
{
	return this->lines[line]->getBounds();
}
CString CTextEditorObject::getHighlightedText()
{
	if (this->hlght || this->hlghting) {

		int i = 0;
		BOOL done = FALSE;

		CString selectedText = L"";

		while (!done && i < this->lines.size()) {

			if (this->lines[i]->isHighlighting()) {

				int j = i;
				BOOL it = TRUE;

				while (it && this->lines[j]->isHighlighting()) {

					selectedText.Append(this->lines[j]->getHighlightedText());
					selectedText.Append(L"\n");

					if (j == this->lines.size() - 1) {
						it = FALSE;
					}
					else {
						j++;
					}
				}

				selectedText = selectedText.Mid(0, selectedText.GetLength() - 1);
				done = TRUE;
			}
			i++;
		}

		return selectedText;
	}
	else {
		return CString();
	}
}
int CTextEditorObject::getLineTextWidth()
{
	int max = this->lines[0]->getText().GetLength();

	for (int i = 1; i < this->lines.size(); i++) {
		if (max < this->lines[i]->getText().GetLength()) {
			max = this->lines[i]->getText().GetLength();
		}
	}

	max *= this->textExtent.cx;
	max += 2;

	return max;
}

int CTextEditorObject::getNumLines()
{
	return this->lines.size();
}

int CTextEditorObject::getBoxHeight(BOOL default)
{
	if (default) {
		return this->defBoxHeight;
	}
	return static_cast<int>(this->defBoxHeight * theApp.zoom);
}

void CTextEditorObject::setSidebar()
{
	if (this->lineNums) {
		this->sidebar.top = this->bounds.top;
		this->sidebar.left = this->bounds.left;
		this->sidebar.bottom = this->bounds.top + this->lines.size() * static_cast<int>(this->defBoxHeight * theApp.zoom);
		this->sidebar.right = this->bounds.left + static_cast<int>(this->defBoxHeight * theApp.zoom);
	}
}
void CTextEditorObject::setHighlighter()
{
	// Find which lines the hlght points lie in
	int i = 1;
	BOOL startP = FALSE, endP = FALSE;
	int endLine = 0;
	int endPos = 0;

	if (this->recentLine != this->activeLine) {
		this->recentLine = this->activeLine;
	}
	if (this->recentPos != this->caretPos) {
		this->recentPos = this->caretPos;
	}
	if (this->recentHlght != this->hlghting) {
		this->recentHlght = this->hlghting;
	}

	while (!(startP && endP) && i <= this->lines.size()) {

		if (!startP && this->hlghtStartP.y >= this->lines[i - 1]->getBounds().top && this->hlghtStartP.y < this->lines[i - 1]->getBounds().bottom && !this->hlghting) {

			this->startLine = i;
			if (this->hlghtStartP.x > (this->lineOffset * theApp.zoom)) {
				this->startPos = (this->hlghtStartP.x - (this->lineOffset * theApp.zoom)) / this->textExtent.cx;
			}
			else {
				this->startPos = 0;
			}

			this->hlghting = TRUE;
			startP = TRUE;
		}

		if (!endP && this->hlghtEndP.y >= this->lines[i - 1]->getBounds().top && this->hlghtEndP.y < this->lines[i - 1]->getBounds().bottom) {

			endLine = i;
			if (this->hlghtEndP.x > (this->lineOffset * theApp.zoom)) {
				endPos = (this->hlghtEndP.x - (this->lineOffset * theApp.zoom)) / this->textExtent.cx;
			}
			else {
				endPos = 0;
			}

			endP = TRUE;
		}

		i++;
	}
	if (!endP) {
		endLine = this->lines.size();
		endPos = this->lines[endLine - 1]->getLength();
	}

	// Unhighlight all lines up to start line and after end line
	for (int i = 0; i < this->startLine - 1; i++) {
		this->lines[i]->setHighlighting(FALSE);
	}

	for (int i = endLine; i < this->lines.size(); i++) {
		this->lines[i]->setHighlighting(FALSE);
	}

	// If we need to highlight over multiple lines
	if (this->startLine != endLine) {

		// Reset the caret on the highlighter
		this->lines[this->activeLine - 1]->setActive(FALSE);
		this->activeLine = endLine;

		// If we need to highlight in reverse
		if (this->startLine > endLine) {

			if (endPos >= this->lines[endLine - 1]->getLength() + 1) {
				endPos = this->lines[endLine - 1]->getLength();
			}

			this->lines[endLine - 1]->setHighlighter(endPos, this->lines[endLine - 1]->getLength() + 1);
			this->lines[endLine - 1]->setHighlighting(TRUE);

			// To make sure the highlight doesn't go past the length of the last line
			if (this->startPos > this->lines[this->startLine - 1]->getLength()) {
				if (this->lines[this->startLine - 1]->getLength() == 0) {
					this->startPos = 2;
				}
				else {
					this->startPos = this->lines[this->startLine - 1]->getLength() + 1;
				}
			}

			this->lines[this->startLine - 1]->setHighlighter(0, this->startPos);
			this->lines[this->startLine - 1]->setHighlighting(TRUE);

			// Highlight all lines inbetween end and start line
			for (int i = endLine + 1; i < this->startLine; i++) {

				this->lines[i - 1]->highlightLine();
				this->lines[i - 1]->setHighlighting(TRUE);
			}
		}
		else {

			this->lines[this->startLine - 1]->setHighlighter(this->startPos, this->lines[this->startLine - 1]->getLength() + 1);
			this->lines[this->startLine - 1]->setHighlighting(TRUE);

			// To make sure the highlight doesn't go past the length of the last line
			if (endPos > this->lines[this->activeLine - 1]->getLength()) {
				if (this->lines[this->activeLine - 1]->getLength() == 0) {
					endPos = 1;
				}
				else {
					endPos = this->lines[this->activeLine - 1]->getLength();
				}
			}

			this->lines[endLine - 1]->setHighlighter(0, endPos);
			this->lines[endLine - 1]->setHighlighting(TRUE);

			// Highlight all lines inbetween start and end line
			for (int i = startLine + 1; i < endLine; i++) {

				this->lines[i - 1]->highlightLine();
				this->lines[i - 1]->setHighlighting(TRUE);
			}
		}
		this->caretPos = endPos;
	}

	// If it is just a one line highlight
	else {

		if (this->startPos > this->lines[this->startLine - 1]->getLength() && endPos > this->lines[this->startLine - 1]->getLength()) {
			this->hlghting = FALSE;
		}

		else {

			if (this->startPos > this->lines[this->startLine - 1]->getLength()) {
				this->startPos = this->lines[this->startLine - 1]->getLength();
			}
			if (endPos > this->lines[this->startLine - 1]->getLength()) {
				endPos = this->lines[this->startLine - 1]->getLength();
				this->caretPos = endPos + 1;
			}
			else {
				this->caretPos = endPos;
			}

			this->lines[this->startLine - 1]->setHighlighter(this->startPos, endPos);
			this->lines[this->startLine - 1]->setHighlighting(TRUE);

			this->activeLine = this->startLine;
		}
	}
}
void CTextEditorObject::setBracketsNull()
{
	for (auto& it : this->brackets) {
		it = std::vector<int>{};
	}
}

BOOL CTextEditorObject::bracketContains(int value, int& itPos, int type, int side)
{
	int i = 0;

	for (auto& it : this->brackets) {

		if (it[side] == value) {
			if (it[2] == type || type == 5) {
				itPos = i;
				return TRUE;
			}
		}
		i++;
	}
	return FALSE;
}
void CTextEditorObject::moveBrackets(int val, int index)
{
	for (auto& bracket : this->brackets) {

		if (index <= bracket[0]) {
			bracket[0] += val;
			bracket[1] += val;
		}
		else if (index <= bracket[1]) {
			bracket[1] += val;
		}
	}
}

