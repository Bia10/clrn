// CircuitPage1.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "CircuitPage1.h"


// CCircuitPage1 dialog

IMPLEMENT_DYNAMIC(CCircuitPage1, CDialog)

CCircuitPage1::CCircuitPage1(CWnd* pParent /*=NULL*/)
	: CDialog(CCircuitPage1::IDD, pParent)
{
	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));
}

CCircuitPage1::~CCircuitPage1()
{
}

void CCircuitPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCircuitPage1, CDialog)
	ON_WM_CTLCOLOR()	// требуется кисть
END_MESSAGE_MAP()

HBRUSH CCircuitPage1::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// вернуть нашу кисть
}
// CCircuitPage1 message handlers
