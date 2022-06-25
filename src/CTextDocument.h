
// CTextDocument.h : interface of the CTextDocument class
//

#pragma once
#include "CTextEditorObject.h"

class CTextDocument : public CDocument
{
public:
    // Public constructors
    CTextDocument() noexcept;
    virtual ~CTextDocument();

    DECLARE_DYNCREATE(CTextDocument);

    // Public overrides
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    virtual void OnFileSave();
    virtual void OnFileOpen();

#ifdef SHARED_HANDLERS
    virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
    virtual void InitializeSearchContent();
#endif // !SHARED_HANDLERS

    // Public implementations
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Document Data
    CArray<CTextEditorObject*> objects;
    int defBoxHeight;
    int lineOffset;

    CString filename;

protected:
    // Protected message handlers
    DECLARE_MESSAGE_MAP()

    // Protected commands
#ifdef SHARED_HANDLERS
    void SetSearchContent(const CString& value);
#endif // !SHARED_HANDLERS
};
