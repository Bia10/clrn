#pragma once


// CCircuitPage1 dialog

class CCircuitPage1 : public CDialog
{
	DECLARE_DYNAMIC(CCircuitPage1)

public:
	CCircuitPage1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCircuitPage1();

// Dialog Data
	enum { IDD = IDD_CIRCUITPAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );// получение кисти 
	DECLARE_MESSAGE_MAP()


	//! Кисть для отрисовки фона
	CBrush m_BackGroundBrush;	
};
