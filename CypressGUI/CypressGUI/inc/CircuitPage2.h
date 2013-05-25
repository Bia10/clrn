#pragma once


// CCircuitPage2 dialog

class CCircuitPage2 : public CDialog
{
	DECLARE_DYNAMIC(CCircuitPage2)

public:
	CCircuitPage2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCircuitPage2();

// Dialog Data
	enum { IDD = IDD_CIRCUITPAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// получение кисти 

	DECLARE_MESSAGE_MAP()

	//! Кисть для отрисовки фона
	CBrush m_BackGroundBrush;	
};
