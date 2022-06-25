
#pragma once
#include "CAppObject.h"
#include "CTextHandler.h"
#include "CTextEditorObject.h"

class CDocView
{
public:
    // Public constructors
    CDocView();
    ~CDocView();

protected:

    // Protected Commands
    virtual void updateWindow(BOOL caret = TRUE, BOOL window = TRUE) = 0;

    // Protected resources
    CTextHandler textInput;
    CFont fFont;

    std::map<CString, CBitmap*> m_pBitmaps;

};

