// src/LogWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "LogWnd.h"


// CLogWnd dialog

IMPLEMENT_DYNAMIC(CLogWnd, CDialog)

CLogWnd::CLogWnd(TResizeFun& ResizeFun, CWnd* pParent /*=NULL*/) : 
	CDialog(CLogWnd::IDD, pParent),
	m_ResizeFun(ResizeFun)
{
	Create(CLogWnd::IDD, pParent);
}

CLogWnd::~CLogWnd()
{
}

void CLogWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogWnd, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLogWnd message handlers

void CLogWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_ResizeFun();
	// TODO: Add your message handler code here
}
