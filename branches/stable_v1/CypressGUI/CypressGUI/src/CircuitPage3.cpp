// CircuitPage3.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "CircuitPage3.h"


// CCircuitPage3 dialog

IMPLEMENT_DYNAMIC(CCircuitPage3, CDialog)

CCircuitPage3::CCircuitPage3(CWnd* pParent /*=NULL*/)
	: CDialog(CCircuitPage3::IDD, pParent)
{
	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));
}

CCircuitPage3::~CCircuitPage3()
{
}

void CCircuitPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCircuitPage3, CDialog)
	ON_WM_CTLCOLOR()	// ��������� �����
END_MESSAGE_MAP()

HBRUSH CCircuitPage3::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// ������� ���� �����
}
// CCircuitPage3 message handlers
