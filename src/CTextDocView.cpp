
// CTextDocView.cpp : implementation of the CTextDocView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DesignArk.h"
#endif

#include "CTextDocument.h"
#include "CTextDocView.h"

#include "CTextEditorObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTextDocView
IMPLEMENT_DYNCREATE(CTextDocView, CScrollView)

BEGIN_MESSAGE_MAP(CTextDocView, CScrollView)

	ON_WM_ERASEBKGND()

	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTextDocView::OnFilePrintPreview)

	ON_WM_HSCROLL()

	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()

	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_EDIT_COPY, &CTextDocView::OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CTextDocView::OnPaste)
	ON_COMMAND(ID_EDIT_CUT, &CTextDocView::OnCut)

END_MESSAGE_MAP()

// CTextDocView construction/destruction

CTextDocView::CTextDocView() noexcept
{
	SetScrollSizes(MM_TEXT, CSize(0, 0));
	this->rgn = new CRgn();
	this->recentBlockLine = 0;

	this->printIterator = 0;
	this->returnNewLines = 0;
	this->recentPrintZoom = theApp.zoom;
	this->recentHlght = FALSE;
	this->printing = FALSE;
}
CTextDocView::~CTextDocView()
{
	delete this->rgn;
}

// Public attributes
CTextDocument* CTextDocView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocument)));
	return (CTextDocument*)m_pDocument;
}
CSize CTextDocView::GetDocSize()
{
	int sizeX = 0, sizeY = 0;

	for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {

		if (sizeX < this->GetDocument()->objects[i]->getLineTextWidth()) {
			sizeX = this->GetDocument()->objects[i]->getLineTextWidth();
		}

		sizeY += this->GetDocument()->objects[i]->getBounds().Height() + (this->GetDocument()->objects[i]->getBoxHeight());
	}

	CRect client;
	GetClientRect(&client);

	sizeX += (client.Width() * 0.1) + static_cast<int>(this->GetDocument()->lineOffset * theApp.zoom);

	sizeY += client.Height() - 
			2*(this->GetDocument()->objects[this->GetDocument()->objects.GetUpperBound()]->getBoxHeight());

	return CSize(sizeX, sizeY);
}

int CTextDocView::getActiveEditor()
{
	return this->activeEditor;
}

// Public implementations
#ifdef _DEBUG
void CTextDocView::AssertValid() const
{
	CView::AssertValid();
}
void CTextDocView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
void CTextDocView::refresh(BOOL caret, BOOL window)
{
	if (caret) {
		this->CreateSolidCaret(this->GetDocument()->objects[this->activeEditor]->getBoxHeight() * 0.05625, this->GetDocument()->objects[this->activeEditor]->getBoxHeight() * 0.8);
	}
	if (window) {
		this->updateWindow(FALSE, TRUE);
	}
}

// Protected implementations
void CTextDocView::updateWindow(BOOL caret, BOOL window)
{
	if (window) {
		CRect client;
		this->GetClientRect(client);
		this->RedrawWindow(client);
	}
	else if (caret) {
		this->SetCaretPos(this->GetDocument()->objects[this->activeEditor]->getCaretPoint(CSize(this->GetDocument()->objects[this->activeEditor]->getBoxHeight() * 0.05, this->GetDocument()->objects[this->activeEditor]->getBoxHeight() * 0.8)) - CSize(this->GetScrollPosition()));
		this->ShowCaret();
	}
}

void CTextDocView::ResyncScrollSizes(BOOL docSize, BOOL reposition)
{
	CSize sizeDoc;
	if (docSize) {
		// Primary stuff with scroll bars
		CClientDC dc(NULL);
		OnPrepareDC(&dc);
		sizeDoc = this->GetDocSize();
		dc.LPtoDP(&sizeDoc);
		SetScrollSizes(MM_TEXT, sizeDoc);
	}

	// Secondary stuff with objects and realignment
	CRect client;
	GetClientRect(&client); // Find current window size

	BOOL redraw = FALSE;

	int tempX = this->GetScrollPosition().x;

	// Is the position of the cursor on the screen
	int activeRight = this->GetDocument()->objects[this->activeEditor]->getBounds().left + static_cast<int>(this->GetDocument()->lineOffset * theApp.zoom) + this->GetDocument()->objects[this->activeEditor]->getCaretPos() * this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx + 7;

	// If the position of the cursor is beyond the edge of the window...
	if (tempX + client.Width() < activeRight && client.Width() > 0) {

		POINT scrollpt;
		scrollpt.x = activeRight - client.Width() + 5; // Move the X to where the scroll is
		scrollpt.y = this->GetScrollPosition().y; // Keep Y the same

		this->ScrollToPosition(scrollpt);
		redraw = TRUE; // We need to redraw the window now

		for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) { // Resize all the objects so that the text fits inside

			this->GetDocument()->objects[i]->OnSize(0, this->GetScrollPosition().x + client.Width(), this->GetScrollPosition().y + client.Height());
		}
	}

	// Is the upper bounds of the active line
	int activeBottom = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top;
	

	if (this->GetScrollPosition().y + client.Height() < activeBottom && client.Height() > 0) {
		
		POINT pt;
		pt.x = this->GetScrollPosition().x;
		pt.y = activeBottom - client.Height();

		this->ScrollToPosition(pt);
		redraw = TRUE;
	} // If the cursor is under the screen, scroll down

	if (!docSize) {
		if (this->GetScrollPosition().x > activeRight && client.Width() > 0) {

			POINT pt;
			pt.x = activeRight - this->GetDocument()->lineOffset - 7;
			pt.y = this->GetScrollPosition().y;

			this->ScrollToPosition(pt);
			redraw = TRUE;
		}
		if (this->GetScrollPosition().y > activeBottom - this->GetDocument()->defBoxHeight && client.Height() > 0) {

			POINT pt;
			pt.x = this->GetScrollPosition().x;
			pt.y = activeBottom - this->GetDocument()->defBoxHeight;

			this->ScrollToPosition(pt);
			redraw = TRUE;
		}
	}

	if (reposition) {

		this->OnSize(0, client.Width(), client.Height());

		CPoint point;
		point.x = 0;
		point.y = 0;

		for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {

			this->GetDocument()->objects[i]->setPosition(point);
			point.y = this->GetDocument()->objects[i]->getBounds().bottom + this->GetDocument()->objects[i]->getBoxHeight();
		}
	}

	if (tempX != this->GetScrollPosition().x) {
		this->expandLineBounds();
	}

	this->updateWindow(FALSE, redraw);
}

// Protected overrides
void CTextDocView::OnDraw(CDC* pDc)
{
	CTextDocument* pDoc = this->GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (!this->printing) {
		this->fFont.CreateFont(static_cast<int>(pDoc->defBoxHeight * theApp.zoom), 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, theApp.sFont);
		pDc->SelectObject(&this->fFont);

	} // If not printing, create a font here

	BOOL finished = FALSE;
	int i = 0; // Iterator
	if (this->printing) { // If we are printing
		i = this->printIterator; // Set the printIterator to where we left off
	}


	while (!finished && i < pDoc->objects.GetSize()) {

		if (this->printing) { // If we are printing, do this

			int bottom = std::get<0>(pDoc->objects[i]->getPrintBounds(returnNewLines, this->pInfo->m_rectDraw.Width())).bottom; // TODO : Remove this bit

			if (bottom < this->pInfo->m_rectDraw.bottom) { // If the object will completley fit on the print page

				this->printIterator++;
				returnNewLines = pDoc->objects[i]->draw(pDc, CSize(), this->GetScrollPosition().x, returnNewLines, this->printing, this->pInfo->m_rectDraw); // Draw object on page as normal
			}
			else { // If not, ie is on the next page in any way

				// If the object should be printed on the next page completley
				if (std::get<0>(pDoc->objects[i]->getPrintBounds(returnNewLines, this->pInfo->m_rectDraw.Width())).top + pDoc->objects[i]->getBoxHeight() >= this->pInfo->m_rectDraw.bottom) {

					pDoc->objects[i]->setPosition(CPoint(this->GetDocument()->objects[i]->getBounds().left, this->pInfo->m_rectDraw.top - (pDoc->objects[i]->getPrintLine() * pDoc->objects[i]->getBoxHeight())));

				}
				else { // If part of the object should be printed on the current page, and the rest on the next

					returnNewLines = pDoc->objects[i]->draw(pDc, CSize(), this->GetScrollPosition().x, returnNewLines, this->printing, this->pInfo->m_rectDraw);
				
					pDoc->objects[i]->setPosition(CPoint(pDoc->objects[i]->getBounds().left, this->pInfo->m_rectDraw.top - ((pDoc->objects[i]->getPrintLine() + returnNewLines) * pDoc->objects[i]->getBoxHeight())));
				}

				for (int j = i + 1; j < pDoc->objects.GetSize(); j++) {
					pDoc->objects[j]->move(0, pDoc->objects[j - 1]->getBounds().bottom + pDoc->objects[j - 1]->getBoxHeight() - pDoc->objects[j]->getBounds().top);
				} // Move the objects relative to the previous object


				this->drawHeader(pDc); // Draw the header for the next page

				finished = TRUE;
			}
		}
		else {
			returnNewLines = pDoc->objects[i]->draw(pDc, CSize(), this->GetScrollPosition().x, this->printing, returnNewLines);
		}

		i++;
	}

	if (this->printing) {

		if (i == pDoc->objects.GetSize()) {
			this->drawHeader(pDc);
		}
	}


	if (!this->printing) {
		this->updateWindow(TRUE, FALSE); // DO NOT CHANGE SECOND PARAMETER TO TRUE!!
		this->fFont.DeleteObject();
	}

	//pDc->FrameRgn(this->rgn, new CBrush(RGB(255, 0, 0)), -1, -1);
	this->rgn = new CRgn();
}
BOOL CTextDocView::OnEraseBkgnd(CDC* pDC)
{
	pDC->SelectClipRgn(this->rgn);
	CView::OnEraseBkgnd(pDC);
	//this->rgn = new CRgn();

	return 0;
}

BOOL CTextDocView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}
BOOL CTextDocView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}
 
void CTextDocView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	theApp.m_ActiveView = this;
	this->refresh();
}

BOOL CTextDocView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return this->DoPreparePrinting(pInfo);
}
void CTextDocView::OnBeginPrinting(CDC* pDc, CPrintInfo* pInfo)
{
	this->printing = TRUE;

	this->recentPrintZoom = theApp.zoom; // Save the current zoom so we can resize after the printing
	theApp.zoom = 5; // TODO : Fix this so that it isn't constant

	this->fFont.CreateFont(static_cast<int>(this->GetDocument()->defBoxHeight * theApp.zoom), 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, theApp.sFont);
	pDc->SelectObject(&this->fFont);
}

void CTextDocView::OnEndPrinting(CDC* pDc, CPrintInfo* pInfo)
{
	theApp.zoom = this->recentPrintZoom;

	CRect client;
	this->GetClientRect(&client);
	this->OnSize(PRINT_SIZE, client.Width(), client.Height());

	this->GetDocument()->objects[0]->setPosition(CPoint(0, 0));

	for (int i = 1; i < this->GetDocument()->objects.GetSize(); i++) {
		this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[i - 1]->getBounds().bottom + this->GetDocument()->objects[i - 1]->getBoxHeight() - this->GetDocument()->objects[i]->getBounds().top);
	} // Move the resized objects

	this->printIterator = 0;
	this->returnNewLines = 0;
	pInfo->m_bContinuePrinting = FALSE;

	this->printing = FALSE;

	this->fFont.DeleteObject();
	CScrollView::OnEndPrinting(pDc, pInfo);
}

void CTextDocView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);
}
void CTextDocView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	UNREFERENCED_PARAMETER(pInfo);

	if (pInfo->m_nCurPage == 1) {// If we have just started printing, start the process of finding the length of the document

		CRect client;
		this->GetClientRect(&client);
		this->OnSize(PRINT_SIZE, client.Width(), client.Height()); // Resize the document and stuff

		pInfo->m_rectDraw.top += 600; // Create a space for the headers
		pInfo->m_rectDraw.bottom -= 300; // Create a space for the margin

		this->GetDocument()->objects[0]->move(0, pInfo->m_rectDraw.top);

		for (int i = 1; i < this->GetDocument()->objects.GetSize(); i++) {
			this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[i-1]->getBounds().bottom + this->GetDocument()->objects[i - 1]->getBoxHeight() - this->GetDocument()->objects[i]->getBounds().top);
		} // Move the resized objects

		std::tuple<CRect, int> printData;
		std::get<0>(printData) = CRect();
		std::get<1>(printData) = 0;

		CSize textExtent = pDC->GetTextExtent(L"A");

		for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {
			this->GetDocument()->objects[i]->setTextExtent(textExtent);
			printData = this->GetDocument()->objects[i]->getPrintBounds(std::get<1>(printData), pInfo->m_rectDraw.Width());
		} // Find the size of the document

		pInfo->SetMaxPage(std::ceil(float(std::get<0>(printData).bottom) / float(pInfo->m_rectDraw.bottom))); // Finally, set the number of pages in the document

	}

	this->pInfo = pInfo; // Save the print info
	this->OnDraw(pDC);

	if (pInfo->GetMaxPage() == pInfo->m_nCurPage) { // Once we have finished printing, resize and reposition all the editors
		theApp.zoom = this->recentPrintZoom;

		CRect client;
		this->GetClientRect(&client);
		this->OnSize(PRINT_SIZE, client.Width(), client.Height());

		this->GetDocument()->objects[0]->setPosition(CPoint(0, 0));

		for (int i = 1; i < this->GetDocument()->objects.GetSize(); i++) {
			this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[i - 1]->getBounds().bottom + this->GetDocument()->objects[i - 1]->getBoxHeight() - this->GetDocument()->objects[i]->getBounds().top);
		} // Move the resized objects

		this->printIterator = 0;
		this->returnNewLines = 0;
		pInfo->m_bContinuePrinting = FALSE;
	}
}

// Protected message handlers
int CTextDocView::OnCreate(LPCREATESTRUCT lpcs)
{
	CView::OnCreate(lpcs);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));

	this->activeEditor = 0;

	this->recentEditor = this->activeEditor;
	this->recentLine = this->GetDocument()->objects[this->activeEditor]->getActiveLine();

	return 0;
}
void CTextDocView::OnInitialUpdate()
{
	ResyncScrollSizes();
	CScrollView::OnInitialUpdate();
}
void CTextDocView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	if (nType != PRINT_SIZE) {
		ResyncScrollSizes();        // ensure that scroll info is up-to-date
	}

	cx += this->GetScrollPosition().x;
	cy += this->GetScrollPosition().y;

	CRect newBounds;

	for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {

		this->GetDocument()->objects[i]->OnSize(nType, max(cx, this->GetDocument()->objects[i]->getLineTextWidth() + static_cast<int>(this->GetDocument()->lineOffset * theApp.zoom)), cy);

		newBounds = this->GetDocument()->objects[i]->getBounds();
		newBounds.right = max(cx, this->GetDocument()->objects[i]->getLineTextWidth() + static_cast<int>(this->GetDocument()->lineOffset * theApp.zoom));

		this->GetDocument()->objects[i]->setBounds(newBounds);
	}

	if (nType != PRINT_SIZE) {
		this->updateWindow();
	}

}

void CTextDocView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

void CTextDocView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

	this->expandLineBounds();

	//this->selectClippingRgn(H_SCROLL);
	this->updateWindow(FALSE, TRUE);
}
void CTextDocView::OnLButtonUp(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	BOOL redraw = this->GetDocument()->objects[this->activeEditor]->OnLButtonUp(nFlags, point);

	std::vector <int> lineNums = this->GetDocument()->objects[this->activeEditor]->iGetLineNum();

	if (lineNums[0] != 0) {

		lineNums.push_back(1);

		BOOL iUpdate = FALSE;
		int i = this->activeEditor+1;

		BOOL exists = FALSE;
		int pos = 1;

		int lineNumSize = lineNums.size();

		// TODO : Possibly change to a binary search to make searching more efficient
		while (!iUpdate && i < this->GetDocument()->objects.GetSize()) { // Iterate through all editors under one that was clicked
			
			if (lineNums == this->GetDocument()->objects[i]->iGetLineNum(1)) { // If the editor requested to be made has already been made, go to that editor and don't create a new one

				this->GetDocument()->objects[this->activeEditor]->setActive(FALSE);
				this->activeEditor = i;
				this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);

				iUpdate = TRUE;
				exists = TRUE;
			}

			else { // If this editor does not match, we must check to see if the requested editor should go before this one, or if we should keep searching

				BOOL jUpdate = FALSE;
				int j = 0;

				while (!jUpdate && j < min(lineNumSize, this->GetDocument()->objects[i]->iGetLineNum(1).size())) { // Iterate through the line numbers' sublines

					if (lineNums[j] < this->GetDocument()->objects[i]->iGetLineNum(1)[j]) { // If this passes, the editor should be above it

						jUpdate = TRUE;
						iUpdate = TRUE;

						pos = i;
					}
					else if(lineNums[j] > this->GetDocument()->objects[i]->iGetLineNum(1)[j]) {
						jUpdate = TRUE;
					}
					j++;
				}


			}

			i++;
		}

		if (!iUpdate) {
			pos = this->GetDocument()->objects.GetSize();
		}

		if (!exists) {

			this->GetDocument()->objects[this->activeEditor]->setActive(FALSE); // Set old editor not active

			int oldEditor = this->activeEditor;
			this->activeEditor = pos; // Change position for editing

			CRect aboveRect = this->GetDocument()->objects[this->activeEditor - 1]->getBounds();

			CRect editorBounds;
			editorBounds.top = aboveRect.bottom + this->GetDocument()->objects[this->activeEditor - 1]->getBoxHeight();
			editorBounds.bottom = editorBounds.top + this->GetDocument()->objects[this->activeEditor - 1]->getBoxHeight();
			editorBounds.left = aboveRect.left;
			editorBounds.right = aboveRect.right;

			std::vector<CString> text = { L"" };

			text[0] = L"// TODO : Add a subline";

			this->GetDocument()->objects.InsertAt(
				this->activeEditor,
				new CTextEditorObject(editorBounds, L"0", TRUE, 0, this->GetDocument()->objects[this->activeEditor - 1]->getBoxHeight(TRUE), TRUE, text, lineNums, this->GetDocument()->lineOffset)); // Add a new line

			CString id;

			id.Format(L"%d", this->activeEditor + 1);
			this->GetDocument()->objects[this->activeEditor]->setID(id);

			for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {

				id.Format(L"%d", i + 1);

				this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[this->activeEditor]->getBounds().Height() + this->GetDocument()->objects[this->activeEditor - 1]->getBoxHeight());
				this->GetDocument()->objects[i]->setID(id);
			} // Move all lines and reset their IDs

			this->selectClippingRgn(EDITOR_EDIT); // Will draw everything under and including the added editor

			// We should have something like this, but it works without it for some reason so it's getting left out for now
			/*CRgn* tempRgn = this->rgn;
			this->rgn = new CRgn();

			this->setClippingRgn(SELECT_LINE);
			this->rgn->CombineRgn(this->rgn, tempRgn, RGN_OR);*/
			
			this->ResyncScrollSizes();
		}
		redraw = TRUE;
	}
	this->updateWindow(FALSE, redraw);
}
void CTextDocView::OnLButtonDown(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	if (this->recentEditor != this->activeEditor) {
		this->recentEditor = this->activeEditor;
	}
	if (this->recentLine != this->GetDocument()->objects[this->recentEditor]->getActiveLine()) {
		this->recentLine = this->GetDocument()->objects[this->recentEditor]->getActiveLine();
	}

	BOOL update = FALSE;
	int i = 0;
	while (!update && i < this->GetDocument()->objects.GetSize()) {

		if (this->GetDocument()->objects[i]->checkPointInBounds(point) || point.y < this->GetDocument()->objects[i]->getBounds().bottom + this->GetDocument()->objects[i]->getBoxHeight()) {
			if (i != this->activeEditor) {

				this->GetDocument()->objects[this->activeEditor]->setActive(FALSE);
				this->activeEditor = i;
				this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);
			}
			update = TRUE;
		}
		i++;
	}

	if (!update) {

		this->GetDocument()->objects[this->activeEditor]->setActive(FALSE);
		this->activeEditor = this->GetDocument()->objects.GetSize() - 1;
		this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);
	}

	BOOL oldHlght = this->recentHlght;

	if (this->recentHlght != this->GetDocument()->objects[this->recentEditor]->hasHighlight()) {
		this->recentHlght = this->GetDocument()->objects[this->recentEditor]->hasHighlight();
	}

	BOOL sidebarbtn = this->GetDocument()->objects[this->activeEditor]->OnLButtonDown(nFlags, point);

	this->selectClippingRgn(SELECT_LINE); // THIS NEEDS TO GO BEFORE THE ADD_RGN_SIDEBAR_BTN

	this->recentBlockLine = this->GetDocument()->objects[this->recentEditor]->getBlockLine();
	int oldEditor = this->activeEditor;
	this->recentEditor = this->activeEditor;

	if (sidebarbtn) { 

		if (this->recentLine == this->recentBlockLine && !oldHlght && !this->GetDocument()->objects[oldEditor]->hasHighlight()) {
			this->selectClippingRgn(ADD_RGN_SIDEBAR_BTN, 0);
		}
		else {
			this->selectClippingRgn(ADD_RGN_SIDEBAR_BTN, 1);
		}
	}
	
	this->updateWindow(FALSE, TRUE);
}
void CTextDocView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	this->GetDocument()->objects[this->activeEditor]->OnLButtonDblClk(nFlags, point);

	for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {
		if (i != this->activeEditor) {

			this->GetDocument()->objects[i]->hlghtingOff();
		}
	}

	this->updateWindow(FALSE, TRUE);
}
void CTextDocView::OnMouseMove(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	int old_cursor = this->GetDocument()->objects[this->activeEditor]->getCursorArrow();

	BOOL objectRedraw = this->GetDocument()->objects[this->activeEditor]->OnMouseMove(nFlags, point);

	if (nFlags == VK_LEFT || objectRedraw) {

		for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {
			if (i != this->activeEditor) {

				this->GetDocument()->objects[i]->hlghtingOff();
			}
		}

		this->selectClippingRgn(HIGHLIGHTING);

		if (this->recentBlockLine != this->GetDocument()->objects[this->recentEditor]->getBlockLine() && this->recentBlockLine != -1) {
			this->selectClippingRgn(ADD_RGN_SIDEBAR_BTN, 1);
			this->recentEditor = this->activeEditor;
			this->recentBlockLine = this->GetDocument()->objects[this->recentEditor]->getBlockLine();
		}
		if (objectRedraw && nFlags != VK_LEFT && this->GetDocument()->objects[this->activeEditor]->getBlockLine() != -1) {
			this->recentBlockLine = this->GetDocument()->objects[this->activeEditor]->getBlockLine();
			this->selectClippingRgn(ADD_RGN_SIDEBAR_BTN, 1);
		}
		this->updateWindow(FALSE, TRUE);
	}

	int new_cursor = this->GetDocument()->objects[this->activeEditor]->getCursorArrow();

	if (new_cursor != old_cursor) {

		if (new_cursor == 0) {

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
		}
		else if (new_cursor == 1) {

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else {

			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		}
	}
}

BOOL CTextDocView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags == MK_CONTROL) {

		BOOL adjust = TRUE;
		
		if ((theApp.zoom <= 0.75 && zDelta < 0) || (theApp.zoom >= 2.8 && zDelta > 0)) {
			adjust = FALSE;
		}

		if (adjust) {

			theApp.zoom += zDelta / 1000.f;
			this->ResyncScrollSizes(TRUE, TRUE);
		}

		this->refresh(TRUE, FALSE);

		return TRUE;
	}
	else {
		return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
	}
}

void CTextDocView::OnRButtonUp(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	this->GetDocument()->objects[this->activeEditor]->OnRButtonUp(nFlags, point);


	if (point.x - this->GetScrollPosition().x > this->GetDocument()->lineOffset) {
		this->ClientToScreen(&point);
		OnContextMenu(this, point);
	}
}
void CTextDocView::OnRButtonDown(UINT nFlags, CPoint point)
{
	point += this->GetScrollPosition();

	if (this->recentEditor != this->activeEditor) {
		this->recentEditor = this->activeEditor;
	}
	if (this->recentLine != this->GetDocument()->objects[this->recentEditor]->getActiveLine()) {
		this->recentLine = this->GetDocument()->objects[this->recentEditor]->getActiveLine();
	}

	BOOL update = FALSE;
	int i = 0;
	while (!update && i < this->GetDocument()->objects.GetSize()) {

		if (this->GetDocument()->objects[i]->checkPointInBounds(point) || point.y < this->GetDocument()->objects[i]->getBounds().bottom + this->GetDocument()->objects[i]->getBoxHeight()) {
			if (i != this->activeEditor) {

				this->GetDocument()->objects[this->activeEditor]->setActive(FALSE);
				this->activeEditor = i;
				this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);
			}
			update = TRUE;
		}
		i++;
	}

	if (!update) {

		this->GetDocument()->objects[this->activeEditor]->setActive(FALSE);
		this->activeEditor = this->GetDocument()->objects.GetSize() - 1;
		this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);
	}

	if (this->recentHlght != this->GetDocument()->objects[this->recentEditor]->hasHighlight()) {
		this->recentHlght = this->GetDocument()->objects[this->recentEditor]->hasHighlight();
	}

	this->GetDocument()->objects[this->activeEditor]->OnRButtonDown(nFlags, point);

	this->selectClippingRgn(SELECT_LINE); // THIS NEEDS TO GO BEFORE THE ADD_RGN_SIDEBAR_BTN

	this->recentEditor = this->activeEditor;

	this->updateWindow(FALSE, TRUE);
}
void CTextDocView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL redraw = TRUE;

	switch (nChar) {

	case VK_RETURN: {
 
		// TODO : Make this more efficient (but make it work first)

		BOOL move = TRUE, increment = TRUE;

		std::vector<int> line = this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getActiveLine() + 1);
		line.push_back(1);
		
		for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {

			if (this->GetDocument()->objects[i]->iGetLineNum(1) == line) {

				if (this->GetDocument()->objects[this->activeEditor]->getCaretPos() == this->GetDocument()->objects[this->activeEditor]->getLineText().GetLength()) {
					increment = FALSE;

				}
				else if (this->GetDocument()->objects[this->activeEditor]->getCaretPos() != 0) {

					move = FALSE;

					// SEND A WARNING MESSAGE
					cout << "UNABLE TO PROCESS COMMAND" << endl;
				}
			}
		}

		if (move) {

			if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {

				this->OnKeyDown(VK_BACK, 1, 0);
			}

			this->GetDocument()->objects[this->activeEditor]->OnRecieveReturn(); // Send the return message to the active editor so it can handle itself

			// Move editors and increment sublines
			for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) { // Iterate through all the editors underneath the active editor

				this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[this->activeEditor]->getBoxHeight()); // Move the editors down the appropriate length

				if (this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() < this->GetDocument()->objects[i]->iGetLineNum(1).size()) { // Check is the line to be changed is bigger, as any line that is the same size of smaller than the avtive editor will never need changed

					if (this->GetDocument()->objects[i]->iGetLineNum(1)[this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1] > /*Get line 1's number of the iterated editor and take the index that is the same as the last index in the active editor line number's*/
						this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).back()) /*Get the active line's number and take the last index*/ {
						// Compare the numbers. See if the editor needs to be incremented by having larger appropriate index

						this->GetDocument()->objects[i]->incrementSublines(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1, 1);
					}
					else if (this->GetDocument()->objects[i]->iGetLineNum(1)[this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1] ==
						this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).back()) {
						// Compare the same numbers as in the previous statement but check for equality
						
						if (increment) { // Check that an increment is appropriate

							this->GetDocument()->objects[i]->incrementSublines(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1, 1);
						}
					}
				}
			}

			this->ResyncScrollSizes();
			this->selectClippingRgn(RETURN_BACK);
		}

		break;
		}

	case VK_BACK: {


		BOOL move = TRUE;
		BOOL removeEditor = FALSE;

		if (this->GetDocument()->objects[this->activeEditor]->getCaretPos() == 0 || this->GetDocument()->objects[this->activeEditor]->isHlghtMultiline()) { // If the the caret is at the start of the line

			std::vector<std::vector<int>> lines = { };

			// Get the line number of the active line
			if (this->GetDocument()->objects[this->activeEditor]->getCaretPos() == 0) {
				lines.push_back(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getActiveLine() + 1));
			}

			if (this->GetDocument()->objects[this->activeEditor]->isHlghtMultiline()) {
				BOOL first = FALSE;
				BOOL last = FALSE;

				int i = 0;

				while (!last && i < this->GetDocument()->objects[this->activeEditor]->getNumLines()) {

					switch (this->GetDocument()->objects[this->activeEditor]->lineHighlight(i)) {

					case 0:
						if (first) {
							last = TRUE;
						}
						break;

					case 1:
						if (first) {
							lines.push_back(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(i + 1));
							last = TRUE;
						}
						else {
							first = TRUE;
						}
						break;

					case 2:
						if (!first) {
							first = TRUE;
						}

						lines.push_back(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(i + 1));
						break;
					}

					i++;
				}
			}

			for (auto& it : lines) {
				it.push_back(1);
			}
			
			// Check if the line wanting to delete has a subline
			for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {

				for (auto& line : lines) {

					if (this->GetDocument()->objects[i]->iGetLineNum(1) == line) {

						move = FALSE;

						// SEND A WARNING MESSAGE
						cout << "UNABLE TO PROCESS COMMAND" << endl;
					}
				}
			}

			// If the line we want to delete does not have a subline...
			if (move) {

				// and we are not on the first line
				if (this->GetDocument()->objects[this->activeEditor]->getActiveLine() != 0) {

					// Move all the editors up one line space and increment them appropriatley
					for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {
						this->GetDocument()->objects[i]->move(0, -this->GetDocument()->objects[this->activeEditor]->getBoxHeight());

						if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {

							if (this->GetDocument()->objects[i]->iGetLineNum(1)[this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1] >
								this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getStartLine()).back()) {

								this->GetDocument()->objects[i]->incrementSublines(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1, -1);
							}
						}

						else if (this->GetDocument()->objects[i]->iGetLineNum(1)[this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1] >
								this->GetDocument()->objects[this->activeEditor]->iGetLineNum(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).back()) {

							this->GetDocument()->objects[i]->incrementSublines(this->GetDocument()->objects[this->activeEditor]->iGetLineNum(1).size() - 1, -1);
						}
					}
				}

				// and there is only one line, but it is not the first editor
				else if (this->GetDocument()->objects[this->activeEditor]->getNumLines() == 1 && this->activeEditor != 0 && !this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {

					// this means that the user want to delete this editor

					this->selectClippingRgn(EDITOR_EDIT); // Will draw everything under and including the removed editor 

					// Remove the editor and move all the editors back

					for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {
						this->GetDocument()->objects[i]->move(0, -(this->GetDocument()->objects[this->activeEditor]->getBounds().Height() + this->GetDocument()->objects[this->activeEditor]->getBoxHeight()));
					}
					this->GetDocument()->objects.RemoveAt(this->activeEditor);

					std::vector<int> oldLine = lines[0];
					lines.pop_back();
					lines.pop_back();

					// TODO : Fix this so the parent line of the deleted editor becomes the active editor
					if (lines.size() > 0) {

						BOOL found = FALSE;
						int i = 0;

						while (!found) {

							found = TRUE;
						}

						this->activeEditor--;
					}

					else {

						this->activeEditor--;
					}

					this->GetDocument()->objects[this->activeEditor]->setActive(TRUE);

					removeEditor = TRUE;

					// Adds the line that needs to be selected to the region
					CRgn rgn2;
					rgn2.CreateRectRgn(	this->GetDocument()->objects[this->activeEditor]->getBounds().left,
										this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top,
										this->GetDocument()->objects[this->activeEditor]->getBounds().right,
										this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).bottom);

					this->rgn->CombineRgn(this->rgn, &rgn2, RGN_OR);
				}
				else if(!this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {
					move = FALSE;
				}
			}
		}
		if (move && !removeEditor) {

			if(this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {

				if (this->GetDocument()->objects[this->activeEditor]->isHlghtMultiline()) {
					this->selectClippingRgn(REMOVE_HIGHLIGHT);
					this->GetDocument()->objects[this->activeEditor]->OnRecieveBackspace();
					this->selectClippingRgn(ADD_RGN_ACTIVE_LINE);
				}
				else {
					this->GetDocument()->objects[this->activeEditor]->OnRecieveBackspace();
					this->selectClippingRgn(TEXT);
					this->selectClippingRgn(ADD_RGN_ACTIVE_LINE);
				}
				
				if (this->GetDocument()->objects.GetSize() > this->activeEditor + 1) {

					int dy = this->GetDocument()->objects[this->activeEditor + 1]->getBounds().top - (this->GetDocument()->objects[this->activeEditor]->getBounds().bottom + this->GetDocument()->objects[this->activeEditor]->getBoxHeight());

					for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {

						this->GetDocument()->objects[i]->move(0, -dy);
					}
				}
			}
			else {

				if (this->GetDocument()->objects[this->activeEditor]->getCaretPos() == 0) {
					// Send backspace to editor AFTER setting the clipping region here
					this->selectClippingRgn(RETURN_BACK);
					this->GetDocument()->objects[this->activeEditor]->OnRecieveBackspace();
				}
				else {
					// Send backspace to editor BEFORE setting the clipping region here
					this->GetDocument()->objects[this->activeEditor]->OnRecieveBackspace();
					this->selectClippingRgn(TEXT);
				}
			}
			this->ResyncScrollSizes();
		}

		break;
	}

	case VK_TAB:
		this->GetDocument()->objects[this->activeEditor]->OnRecieveTab();
		break;

	default:
		if (this->textInput.OnKeyDown(nChar, nRepCnt, nFlags)) {
			if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {

				this->OnKeyDown(VK_BACK, 1, 0);
			}
			else {

				// Set the clipping region for basic text input, with no highlighting
				this->selectClippingRgn(TEXT);
			}

			this->GetDocument()->objects[this->activeEditor]->OnRecieveText(this->textInput.RecieveText());

			

			this->ResyncScrollSizes();
		}
		else {
			int oldLine = this->GetDocument()->objects[this->activeEditor]->getActiveLine();
			int oldCaretPos = this->GetDocument()->objects[this->activeEditor]->getCaretPos();

			this->GetDocument()->objects[this->activeEditor]->OnKeyDown(nChar, nRepCnt, nFlags);

			if (oldLine != this->GetDocument()->objects[this->activeEditor]->getActiveLine()) {
				this->recentLine = oldLine;
				this->selectClippingRgn(SELECT_LINE);
			}
			else if (oldCaretPos != this->GetDocument()->objects[this->activeEditor]->getCaretPos()) {
				this->selectClippingRgn(CARET_MOVE);
				redraw = FALSE;
			}

			this->ResyncScrollSizes(FALSE, TRUE);
		}

		break;
	}

	if (redraw) {
		this->updateWindow(FALSE, TRUE);
	}
}
void CTextDocView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	this->textInput.OnKeyUp(nChar, nRepCnt, nFlags);
}

void CTextDocView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	point -= this->GetScrollPosition();

	this->GetDocument()->objects[this->activeEditor]->OnContextMenu(pWnd, point);
}

void CTextDocView::OnCopy()
{
	if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {
		if (!OpenClipboard())
		{
			AfxMessageBox(_T("Cannot open the Clipboard"));
			return;
		}

		// Remove the current Clipboard contents
		if (!EmptyClipboard())
		{
			AfxMessageBox(_T("Cannot empty the Clipboard"));
			return;
		}

		// Get the currently selected data
		CStringA text(this->GetDocument()->objects[this->activeEditor]->getHighlightedText());

		const char* x = text;

		HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.GetLength() * 2);

		strcpy_s((char*)hGlob, text.GetLength() * 2, x);

		// For the appropriate data formats...
		if (::SetClipboardData(CF_TEXT, hGlob) == NULL)
		{
			CString msg;
			msg.Format(_T("Unable to set Clipboard data, error: %d"), GetLastError());
			AfxMessageBox(msg);
			CloseClipboard();
			GlobalFree(hGlob);
			return;
		}
		CloseClipboard();
	}
}
void CTextDocView::OnPaste()
{
	HANDLE hGlob;

	// If the application is in edit mode, 
	// get the clipboard text. 

	if (!IsClipboardFormatAvailable(CF_TEXT)) {
		return;
	}

	if (!OpenClipboard()) {
		AfxMessageBox(_T("Cannot open the Clipboard"));
		return;
	}

	hGlob = GetClipboardData(CF_TEXT);
	if (hGlob != NULL)
	{
		if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {
			this->GetDocument()->objects[this->activeEditor]->OnRecieveBackspace();
		}

		this->recentLine = this->GetDocument()->objects[this->activeEditor]->getActiveLine();

		int carridgeReturn = this->GetDocument()->objects[this->activeEditor]->OnRecieveText(CString((char*)GlobalLock(hGlob)), TRUE);
		
		for (int i = this->activeEditor + 1; i < this->GetDocument()->objects.GetSize(); i++) {
			
			this->GetDocument()->objects[i]->move(0, this->GetDocument()->objects[this->activeEditor]->getBoxHeight() * carridgeReturn);
		}
		GlobalUnlock(hGlob);
	}
	else {
		AfxMessageBox(_T("Clipboard is Empty"));
		return;
	}
	CloseClipboard();

	this->selectClippingRgn(PASTE);

	this->ResyncScrollSizes(TRUE, TRUE);
	this->updateWindow(FALSE, TRUE);

	return;
}
void CTextDocView::OnCut()
{
	this->OnCopy();
	if (this->GetDocument()->objects[this->activeEditor]->hasHighlight()) {
		this->OnKeyDown(VK_BACK, 1, 0);
	}
}

void CTextDocView::expandLineBounds()
{
	CRect client;
	this->GetClientRect(&client);

	CRect newBounds;

	for (int i = 0; i < this->GetDocument()->objects.GetSize(); i++) {

		newBounds = this->GetDocument()->objects[i]->getBounds();
		newBounds.right = max(client.right + this->GetScrollPosition().x, this->GetDocument()->objects[i]->getLineTextWidth() + static_cast<int>(this->GetDocument()->lineOffset * theApp.zoom));

		this->GetDocument()->objects[i]->setBounds(newBounds);
	}
}

void CTextDocView::selectClippingRgn(int nAction, int type)
{
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

	switch (nAction) {

	case TEXT: { // Working

		if (this->recentHlght) {
			x2 = this->GetDocument()->objects[this->activeEditor]->getBounds().left + this->GetDocument()->lineOffset + ((this->GetDocument()->objects[this->activeEditor]->getLineText(this->GetDocument()->objects[this->activeEditor]->getActiveLine() + 1).GetLength() + 1) * this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx) - this->GetScrollPosition().x;
		}
		else {
			x2 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).right - this->GetScrollPosition().x;
		}

		int drawpos = this->GetDocument()->objects[this->activeEditor]->getCaretPos();

		int i = this->GetDocument()->objects[this->activeEditor]->getCaretPos();
		BOOL end = FALSE;

		while (i >= 0 && !end) {
			
			if (this->GetDocument()->objects[this->activeEditor]->getLineText().Mid(i, 1) == L" " || i == 0) {
				drawpos = i;
				end = TRUE;
			}

			i--;
		}

		x1 = this->GetDocument()->objects[this->activeEditor]->getBounds().left + this->GetDocument()->lineOffset + (drawpos * this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx) - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top - this->GetScrollPosition().y;
		y2 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).bottom - this->GetScrollPosition().y;

		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;
	}

	case RETURN_BACK: // Working

		x1 = this->GetDocument()->objects[this->activeEditor]->getBounds().left - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()-1).top - this->GetScrollPosition().y;
		x2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().right - this->GetScrollPosition().x;
		y2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().bottom + this->GetDocument()->objects[this->activeEditor]->getBoxHeight() - this->GetScrollPosition().y;
		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;

	case EDITOR_EDIT: { // Working
		x1 = this->GetDocument()->objects[this->activeEditor]->getBounds().left - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getBounds().top - this->GetScrollPosition().y;
		x2 = this->GetDocument()->objects[this->activeEditor]->getBounds().right - this->GetScrollPosition().x;
		y2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().bottom - this->GetScrollPosition().y;
		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));

		CRgn* rgn1 = new CRgn();
		CRect tempRect = this->GetDocument()->objects[this->recentEditor]->getLineBounds(this->GetDocument()->objects[this->recentEditor]->getActiveLine());

		x1 = tempRect.left;
		y1 = tempRect.top;
		x2 = tempRect.right;
		y2 = tempRect.bottom;

		VERIFY(rgn1->CreateRectRgn(x1, y1, x2, y2));
		this->rgn->CombineRgn(this->rgn, rgn1, RGN_OR);

		this->selectClippingRgn(ADD_RGN_SIDEBAR_BTN, 1);

		break;
	}

	case HIGHLIGHTING: { // Working
		CRect temp = this->GetDocument()->objects[this->activeEditor]->getHighlightClippingRect();
		x1 = temp.left - this->GetScrollPosition().x;
		y1 = temp.top - this->GetScrollPosition().y;
		x2 = temp.right - this->GetScrollPosition().x;
		y2 = temp.bottom - this->GetScrollPosition().y;
		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;
	}

	case SELECT_LINE: { // Working

		if (this->GetDocument()->objects[this->activeEditor]->getActiveLine() != this->recentLine) {
			CRgn* rgn1 = new CRgn();
			CRgn* rgn2 = new CRgn();
			CRgn* tempRgn = new CRgn();

			CRect temp = this->GetDocument()->objects[this->recentEditor]->getLineBounds(this->recentLine);

			x1 = temp.left - this->GetScrollPosition().x;
			y1 = temp.top - this->GetScrollPosition().y;
			x2 = temp.right - this->GetScrollPosition().x;
			y2 = temp.bottom - this->GetScrollPosition().y;

			VERIFY(rgn1->CreateRectRgn(x1, y1, x2, y2));

			x1 += 1;
			y1 += 1;
			x2 -= 1;
			y2 -= 1;

			VERIFY(tempRgn->CreateRectRgn(x1, y1, x2, y2));

			int nCombineResult = rgn1->CombineRgn(rgn1, tempRgn, RGN_XOR);
			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);

			tempRgn = new CRgn();

			x1 = this->GetCaretPos().x;
			x2 = x1 + this->GetDocument()->objects[0]->getTextExtent().cx*0.25;

			VERIFY(tempRgn->CreateRectRgn(x1, y1, x2, y2));

			nCombineResult = rgn1->CombineRgn(rgn1, tempRgn, RGN_XOR);
			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);

			tempRgn = new CRgn();

			temp = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine());

			x1 = temp.left - this->GetScrollPosition().x;
			y1 = temp.top - this->GetScrollPosition().y;
			x2 = temp.right - this->GetScrollPosition().x;
			y2 = temp.bottom - this->GetScrollPosition().y;

			VERIFY(rgn2->CreateRectRgn(x1, y1, x2, y2));

			x1 += 1;
			y1 += 1;
			x2 -= 1;
			y2 -= 1;

			VERIFY(tempRgn->CreateRectRgn(x1, y1, x2, y2));

			nCombineResult = rgn2->CombineRgn(rgn2, tempRgn, RGN_XOR);
			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);

			VERIFY(this->rgn->CreateRectRgn(0, 0, 0, 0)); // Must initialise the rgn first
			nCombineResult = this->rgn->CombineRgn(rgn1, rgn2, RGN_OR);

			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);

			if (this->recentHlght) {

				CRgn* rgn3 = this->GetDocument()->objects[this->recentEditor]->getHighlightExactRgn(this->GetScrollPosition().x, this->GetScrollPosition().y);

				nCombineResult = this->rgn->CombineRgn(this->rgn, rgn3, RGN_OR);

				ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);
			}

		}
		
		else if (this->recentHlght) {

			CRect temp = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine());

			x1 = temp.left - this->GetScrollPosition().x;
			y1 = temp.top - this->GetScrollPosition().y;
			x2 = temp.right - this->GetScrollPosition().x;
			y2 = temp.bottom - this->GetScrollPosition().y;

			VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));

			CRgn* rgn3 = this->GetDocument()->objects[this->recentEditor]->getHighlightExactRgn(this->GetScrollPosition().x, this->GetScrollPosition().y);

			int nCombineResult = this->rgn->CombineRgn(this->rgn, rgn3, RGN_OR);

			ASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION);
		}

		else {

			x1 = (this->GetDocument()->objects[this->activeEditor]->getRecentPos() - 0.5) * this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx ;
			y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top;
			x2 = x1 + this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx;
			y2 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).bottom;

			this->rgn->CreateRectRgn(x1, y1, x2, y2);
		}

		break;
	}

	case REMOVE_HIGHLIGHT: // Working
		x1 = this->GetDocument()->objects[this->activeEditor]->getBounds().left - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(min(this->GetDocument()->objects[this->activeEditor]->getActiveLine(), this->GetDocument()->objects[this->activeEditor]->getStartLine())).top - this->GetScrollPosition().y;
		x2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().right - this->GetScrollPosition().x;
		y2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().bottom - this->GetScrollPosition().y;
		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;

	case CARET_MOVE: // Working

		x1 = this->GetCaretPos().x - this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top - this->GetScrollPosition().y;
		x2 = this->GetCaretPos().x + this->GetDocument()->objects[this->activeEditor]->getTextExtent().cx - this->GetScrollPosition().x;
		y2 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).bottom - this->GetScrollPosition().y;

		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;

	case ADD_RGN_SIDEBAR_BTN: { // Working

		if (type != 0 && type != 1) {
			throw("ERROR : type error. type must be 0 or 1");
		}

		Gdiplus::Bitmap bitmap(L"res/arrow_right_click.bmp");

		x1 = this->GetDocument()->objects[this->recentEditor]->getBounds().left + static_cast<int>(this->GetDocument()->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetWidth() * theApp.zoom) / 2;
		y1 = this->GetDocument()->objects[this->recentEditor]->getLineBounds(this->recentBlockLine).top + static_cast<int>(this->GetDocument()->defBoxHeight * theApp.zoom) / 2 - (bitmap.GetHeight() * theApp.zoom) / 2 - this->GetScrollPosition().y;
		x2 = x1 + (bitmap.GetWidth() * theApp.zoom);
		y2 = y1 + (bitmap.GetHeight() * theApp.zoom) - this->GetScrollPosition().y;

		if (type == 0) {
			VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		}
		else {
			CRgn* rgn1 = new CRgn();
			VERIFY(rgn1->CreateRectRgn(x1, y1, x2, y2));
			this->rgn->CombineRgn(this->rgn, rgn1, RGN_OR);
		}

		break;
	}
	case ADD_RGN_ACTIVE_LINE: {
		CRgn* rgn1 = new CRgn();
		CRect temp = this->GetDocument()->objects[this->recentEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine());

		x1 = temp.left - this->GetScrollPosition().x;
		y1 = temp.top - this->GetScrollPosition().y;
		x2 = temp.right - this->GetScrollPosition().x;
		y2 = temp.bottom - this->GetScrollPosition().y;

		VERIFY(rgn1->CreateRectRgn(x1, y1, x2, y2));

		this->rgn->CombineRgn(this->rgn, rgn1, RGN_OR);

		break;
	}

	case H_SCROLL:
		
		x1 = 0;
		y1 = this->GetDocument()->objects[0]->getBounds().top;
		x2 = this->GetScrollPosition().x + this->GetDocument()->lineOffset;
		y2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().bottom;
		
		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;

	case PASTE:

		x1 = this->GetDocument()->objects[this->activeEditor]->getBounds().left - this->GetScrollPosition().x;
		y1 = this->GetDocument()->objects[this->activeEditor]->getLineBounds(this->GetDocument()->objects[this->activeEditor]->getActiveLine()).top - this->GetScrollPosition().y;
		x2 = this->GetDocument()->objects[this->activeEditor]->getBounds().right - this->GetScrollPosition().x;
		y2 = this->GetDocument()->objects[this->GetDocument()->objects.GetSize() - 1]->getBounds().bottom - this->GetScrollPosition().y;

		VERIFY(this->rgn->CreateRectRgn(x1, y1, x2, y2));
		break;
	}
	//cout << x1 << ", " << y1 << ", " << x2 << ", " << y2 << endl;// KEEP THIS FOR DEBUGGING
}

void CTextDocView::drawHeader(CDC* pDc)
{
	pDc->TextOut(this->GetDocument()->objects[0]->getLineBounds().left, 400 - this->GetDocument()->objects[0]->getTextExtent().cy,
		this->GetDocument()->filename, this->GetDocument()->filename.GetLength());

	CString page;
	page.Format(L"%d", this->pInfo->m_nCurPage);

	pDc->TextOut(this->pInfo->m_rectDraw.Width() - (this->GetDocument()->objects[0]->getLineBounds().left - this->GetDocument()->objects[0]->getBounds().left) / 2 - page.GetLength()* this->GetDocument()->objects[0]->getTextExtent().cx, 400 - this->GetDocument()->objects[0]->getTextExtent().cy, page, page.GetLength());

	pDc->MoveTo(CPoint((this->GetDocument()->objects[0]->getLineBounds().left - this->GetDocument()->objects[0]->getBounds().left)/2, 400));
	pDc->LineTo(CPoint(this->pInfo->m_rectDraw.Width() - (this->GetDocument()->objects[0]->getLineBounds().left - this->GetDocument()->objects[0]->getBounds().left) / 2, 400));
} 
