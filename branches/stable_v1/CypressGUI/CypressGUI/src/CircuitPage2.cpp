// CircuitPage2.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "CircuitPage2.h"


// CCircuitPage2 dialog

IMPLEMENT_DYNAMIC(CCircuitPage2, CDialog)

CCircuitPage2::CCircuitPage2(CWnd* pParent /*=NULL*/)
	: CDialog(CCircuitPage2::IDD, pParent)
{
	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));
}

CCircuitPage2::~CCircuitPage2()
{
}

void CCircuitPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCircuitPage2, CDialog)
	ON_WM_CTLCOLOR()	// требуется кисть
END_MESSAGE_MAP()

HBRUSH CCircuitPage2::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// вернуть нашу кисть
}

// CCircuitPage2 message handlers
