// OneControl.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "OneControl.h"

#include <string>

#include <boost/lexical_cast.hpp>

// COneControl dialog

IMPLEMENT_DYNAMIC(COneControl, CDialog)

COneControl::COneControl(CWnd* pParent /*=NULL*/)
	: CDialog(COneControl::IDD, pParent)
	, m_nSlider(0)
{
	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));
}

COneControl::~COneControl()
{
}

HBRUSH COneControl::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// вернуть нашу кисть
}


void COneControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDERF, m_nSlider);
}


BEGIN_MESSAGE_MAP(COneControl, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERF, &COneControl::OnNMCustomdrawSliderf)
	ON_EN_KILLFOCUS(IDC_INDICATOR_F, &COneControl::OnEnKillfocusIndicatorF)
	ON_WM_CTLCOLOR()	// требуется кисть
END_MESSAGE_MAP()


// COneControl message handlers

void COneControl::OnNMCustomdrawSliderf(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRY_EXCEPTIONS
	{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		// TODO: Add your control notification handler code here
		*pResult = 0;

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF));
		Slider->SetRange(0, 1000000);
		m_nSlider = Slider->GetPos();

		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F));

		const std::wstring sValue = boost::lexical_cast< std::wstring > ((1000000 - m_nSlider));
		EditBox->SetWindowText(sValue.c_str());
	}
	CATCH_IGNORE_EXCEPTIONS
}

void COneControl::OnEnKillfocusIndicatorF()
{
	TRY_EXCEPTIONS
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F));

		CString sValue;
		EditBox->GetWindowText(sValue);

		m_nSlider = boost::lexical_cast< int > (sValue.GetBuffer());

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF));
		Slider->SetPos((1000000 - m_nSlider));
		const int nTest = Slider->GetPos();
	}
	CATCH_IGNORE_EXCEPTIONS
}

const int COneControl::GetFrequency() const
{
	TRY_EXCEPTIONS
	{
		CEdit * pEdit = reinterpret_cast< CEdit * >(GetDlgItem(IDC_INDICATOR_F));
		CString sText;
		pEdit->GetWindowText(sText);
		return boost::lexical_cast< int > (sText.GetBuffer());
	}
	catch(std::exception&)
	{
		return 1;
	}
}
