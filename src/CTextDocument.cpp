
// CTextDocument.cpp : implementation of the CTextDocument class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DesignArk.h"
#endif

#include "CTextDocument.h"
#include "ChildFrm.h"
#include "CTextDocView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTextDocument

IMPLEMENT_DYNCREATE(CTextDocument, CDocument)

BEGIN_MESSAGE_MAP(CTextDocument, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CTextDocument::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CTextDocument::OnFileOpen)
END_MESSAGE_MAP()

// Public constructors
CTextDocument::CTextDocument() noexcept
{
	this->SetTitle(L"Pseudocode Editor");
	std::vector<CString> text = { L"" };

	this->defBoxHeight = 20;
	this->lineOffset = 100;
	this->filename = L"local";

	CRect editorBounds;
	editorBounds.top = 0;
	editorBounds.bottom = this->defBoxHeight*theApp.zoom * (text.size());
	editorBounds.left = 0;
	editorBounds.right = 1000;

	std::vector <int> lineNums = { 1 };

	this->objects.Add(new CTextEditorObject(editorBounds, L"1", TRUE, 0, this->defBoxHeight*theApp.zoom, TRUE, text, lineNums, this->lineOffset));
}
CTextDocument::~CTextDocument()
{
	for (int i = 0; i < this->objects.GetSize(); i++) {
		delete this->objects[i];
	}
}

// Public overrides
BOOL CTextDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}
void CTextDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{

	}
	else
	{
		
	}
}

void CTextDocument::OnFileSave()
{
	const TCHAR szFilter[] = _T("Text Files(*.txt) | *.txt | All File | *.* | ");

	CFileDialog fileDialog(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

	if (fileDialog.DoModal() == IDOK) {

		CFile file;

		if (file.Open(fileDialog.GetFolderPath() + L"\\" + fileDialog.GetFileName(), CFile::modeCreate | CFile::modeWrite)) {

			CString data = L"";

			int max_size = this->objects[0]->sGetLineNum(this->objects[0]->getNumLines()).GetLength();

			for (int i = 1; i < this->objects.GetSize(); i++) {
				if (this->objects[i]->sGetLineNum(1).GetLength() > max_size) {
					max_size = this->objects[i]->sGetLineNum(this->objects[i]->getNumLines()).GetLength();
				}
			}

			for (int i = 0; i < this->objects.GetSize(); i++) {
				for (int j = 1; j < this->objects[i]->getNumLines() + 1; j++) {

					for (int k = 0; k < (max_size - this->objects[i]->sGetLineNum(j).GetLength()); k++) {
						data.Append(L" ");
					}

					data.Append(this->objects[i]->sGetLineNum(j));
					data.Append(L"    ");
					data.Append(this->objects[i]->getLineText(j));
					data.Append(L"\n");
				}
				data.Append(L"\n\n");
			}
			
			file.Write(data.GetBuffer(), data.GetLength()*2);

			this->filename = fileDialog.GetFileName();
		}
	}
}
void CTextDocument::OnFileOpen()
{
	const TCHAR szFilter[] = _T("Text Files (*.txt)|*.txt| All File|*.*|"); // Set a filter for file types

	CFileDialog fileDialog(TRUE, _T("*.txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter); // Instantiate file dialog box

	if (fileDialog.DoModal() == IDOK) { // Open the file dialog and wait for the user to be done

		CString filePath = fileDialog.GetFolderPath() + L"\\" + fileDialog.GetFileName(); // Create the file path
		CDocument* newDoc = theApp.OpenDocumentFile(filePath); // Open a new document for the file

		CFile file; // Open the file pathway

		if (file.Open(filePath, CFile::modeRead)) { // Read through the file

			// Read the text from the file

			int iFileSiz = file.GetLength();  // Getting the content length
			BYTE* pData = new BYTE[iFileSiz]; // To save the data in

			file.Read(pData, iFileSiz);       //  Reading file content

			pData[iFileSiz] = '\0';           //  Add last character as NULL

			file.Close(); // Close the file

			CString character;
			CString data;

			int k = 0;

			for (int i = 0; i < iFileSiz; i++) { // Iterate through all the readable characters

				character.Format(L"%C", pData[i]); // Format each character correctlyS
				data += character; // Add the character to the current line (data)

				if (character == L"\n" || i == iFileSiz-1) { // Once we have recieved a new space or have gotten to the end of the file

					int j = 0;
					BOOL end = FALSE; // Iterators

					int space = 0;
					BOOL startNum = FALSE;

					CString subnum = L"";
					std::vector<int>writingLineNum = {};

					while (j < data.GetLength() && !end) { // Iterate through the collected line

						CString active = data.Mid(j, 1); // Is the secondary iterated character

						if (active == L"0" ||
							active == L"1" || 
							active == L"2" || 
							active == L"3" || 
							active == L"4" || 
							active == L"5" || 
							active == L"6" || 
							active == L"7" || 
							active == L"8" || 
							active == L"9") { // Once we have found a number ie, where the line number is in the data

							j++;
							startNum = TRUE; // This tells us that we have found start of the line number

							subnum.Append(active);
						}
						else if (active == L"." && space == 0) { // Once we have reached the next subline
							j++;
							writingLineNum.push_back(_ttoi(subnum)); 
							subnum = L"";
						}
						else if (active == L" " && space < 4/*There should be 4 spaces between the end of the line number and the start of the text*/) { // If we have not reached the start of the actual text, either before or after the subline

							j++;

							if (startNum) { // If we have found the start of the line number we can start incrementing space

								space++;
							}
						}
						else { // This means that we have gotten to the start of the text

							if (subnum != L"") {
								writingLineNum.push_back(_ttoi(subnum));
							}
							subnum = L"";
							end = TRUE;
						}
					}


					BOOL newEdit = FALSE;

					if (writingLineNum.size() > 0) { // If there is not a gap in the line numbers

						std::vector<int>writingTemp = writingLineNum;
						writingTemp.pop_back(); // Remove the last subline in order to search for a parent line

						std::vector<int>itTemp;
						int i = ((CTextDocument*)newDoc)->objects.GetSize() - 1; // Find the number of objects created so far
						BOOL fin = FALSE; // Iterators

						while (i >= 0 && !fin) { // Iterate through all the objects in the new doc, in reverse

							itTemp = ((CTextDocument*)newDoc)->objects[i]->iGetLineNum(1);
							itTemp.pop_back(); // Get the line number and remove the recent subline

							if (writingTemp == itTemp) { // Check if this is the editor we are searching

								fin = TRUE; // If it is, say we found it with this
							}

							i--;
						}

						if (!fin) {
							newEdit = TRUE; // If we did not find an editor that our line belongs to, create a new one
						}

						if (newEdit) {

							// If we get here, it means that the new line should be made in a new editor

							CRect prevBounds = ((CTextDocument*)newDoc)->objects[k]->getBounds(); // Find the bounds of the editor that lies above the one we are about to create

							CRect bounds = prevBounds; // Create bounds for the new object
							bounds.top = prevBounds.bottom + ((CTextDocument*)newDoc)->objects[k]->getBoxHeight(); // Move the top
							bounds.bottom = bounds.top + ((CTextDocument*)newDoc)->objects[k]->getBoxHeight(); // Move the bottom

							CString id;
							id.Format(L"%d", k + 1);

							((CTextDocument*)newDoc)->objects[k]->OnRecieveBackspace();

							((CTextDocument*)newDoc)->objects.Add(
								new CTextEditorObject(bounds, id, TRUE, 0, ((CTextDocument*)newDoc)->defBoxHeight, FALSE,
									std::vector<CString>{data.Mid(j)}, writingLineNum,
									this->lineOffset)
							);

							((CTextDocument*)newDoc)->objects[k + 1]->OnRecieveReturn(TRUE);
							((CTextDocument*)newDoc)->objects[k]->setActive(FALSE);
							k++;
							//((CTextDocument*)newDoc)->objects[k]->OnRecieveReturn(TRUE);
						}
						else {
							((CTextDocument*)newDoc)->objects[k]->OnRecieveText(data.Mid(j), TRUE); // Send the data through to the current editor
							//((CTextDocument*)newDoc)->objects[k]->OnRecieveReturn(TRUE);

						}
					}

					data = L"";
				}
			}

			((CTextDocument*)newDoc)->objects[k]->OnRecieveBackspace();
			((CTextDocument*)newDoc)->objects[k]->setActive(FALSE);
			((CTextDocument*)newDoc)->objects[0]->setActive(TRUE);

			((CTextDocView*)theApp.m_ActiveView)->refresh();

			((CTextDocument*)newDoc)->filename = fileDialog.GetFileName();
		}
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CTextDocument::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(0, 0, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CTextDocument::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTextDocument::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Public implementations
#ifdef _DEBUG
void CTextDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

