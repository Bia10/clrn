#pragma once


// CCircuitPage3 dialog

class CCircuitPage3 : public CDialog
{
	DECLARE_DYNAMIC(CCircuitPage3)

public:
	CCircuitPage3(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCircuitPage3();

// Dialog Data
	enum { IDD = IDD_CIRCUITPAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// получение кисти 

	DECLARE_MESSAGE_MAP()

	//! Кисть для отрисовки фона
	CBrush m_BackGroundBrush;	
};
