
#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "DesignArk.h"
#endif

#include "CDocView.h"


// Public constructors
CDocView::CDocView()
{
	this->m_pBitmaps[L"caret"] = new CBitmap();
	this->m_pBitmaps[L"caret"]->LoadBitmap(IDB_CARET);
}
CDocView::~CDocView()
{

	for (auto& it : this->m_pBitmaps) {
		delete it.second;
	}
}

void CDocView::updateWindow(BOOL caret, BOOL window)
{
}
