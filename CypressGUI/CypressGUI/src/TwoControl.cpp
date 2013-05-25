// TwoControl.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "TwoControl.h"

#include <string>

#include <boost/lexical_cast.hpp>

#include <math.h>


// CTwoControl dialog

IMPLEMENT_DYNAMIC(CTwoControl, CDialog)

CTwoControl::CTwoControl(CWnd* pParent /*=NULL*/)
	: CDialog(CTwoControl::IDD, pParent)
	, m_nSlider2(0)
	, m_nSlider1(0)
	, m_nStep(1000)
{
	m_BackGroundBrush.CreateSolidBrush(RGB(212, 208, 200));
}

CTwoControl::~CTwoControl()
{
}

HBRUSH CTwoControl::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{	
	return m_BackGroundBrush;	// вернуть нашу кисть
}


void CTwoControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDERF2, m_nSlider2);
	DDX_Slider(pDX, IDC_SLIDERF1, m_nSlider1);
	DDX_Text(pDX, IDC_STEP, m_nStep);
	DDV_MinMaxInt(pDX, m_nStep, 1, 100000);
}


BEGIN_MESSAGE_MAP(CTwoControl, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERF1, &CTwoControl::OnNMCustomdrawSliderf1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERF2, &CTwoControl::OnNMCustomdrawSliderf2)
	ON_EN_KILLFOCUS(IDC_INDICATOR_F1, &CTwoControl::OnEnKillfocusIndicatorF1)
	ON_EN_KILLFOCUS(IDC_INDICATOR_F2, &CTwoControl::OnEnKillfocusIndicatorF2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERF2, &CTwoControl::OnNMReleasedcaptureSliderf2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDERF1, &CTwoControl::OnNMReleasedcaptureSliderf1)
	ON_EN_CHANGE(IDC_STEP, &CTwoControl::OnEnChangeStep)
	ON_WM_CTLCOLOR()	// требуется кисть
END_MESSAGE_MAP()


// CTwoControl message handlers

void CTwoControl::OnNMCustomdrawSliderf1(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRY_EXCEPTIONS
	{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		// TODO: Add your control notification handler code here
		*pResult = 0;

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF1));
		Slider->SetRange(0, 1000000);
		m_nSlider1 = Slider->GetPos();

		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F1));
		
		const std::wstring sValue = boost::lexical_cast< std::wstring > ((1000000 - m_nSlider1));
		EditBox->SetWindowText(sValue.c_str());

		{
			CSliderCtrl *Slider;
			Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF2));
			
		}
	}
	CATCH_IGNORE_EXCEPTIONS
}

void CTwoControl::OnNMCustomdrawSliderf2(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRY_EXCEPTIONS
	{
		LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
		// TODO: Add your control notification handler code here
		*pResult = 0;

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF2));
		Slider->SetRange(0, 1000000);
		m_nSlider2 = Slider->GetPos();


		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F2));

		const std::wstring sValue = boost::lexical_cast< std::wstring > ((1000000 - m_nSlider2));
		EditBox->SetWindowText(sValue.c_str());
	}
	CATCH_IGNORE_EXCEPTIONS
}


void CTwoControl::OnEnKillfocusIndicatorF1()
{
	TRY_EXCEPTIONS
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F1));

		CString sValue;
		EditBox->GetWindowText(sValue);

		m_nSlider1 = boost::lexical_cast< int > (sValue.GetBuffer());

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF1));
		Slider->SetPos((1000000 - m_nSlider1));
		const int nTest = Slider->GetPos();
	}
	CATCH_IGNORE_EXCEPTIONS
}

void CTwoControl::OnEnKillfocusIndicatorF2()
{
	TRY_EXCEPTIONS
	{
		CEdit * EditBox;
		EditBox = reinterpret_cast<CEdit *>(GetDlgItem(IDC_INDICATOR_F2));

		CString sValue;
		EditBox->GetWindowText(sValue);

		m_nSlider2 = boost::lexical_cast< int > (sValue.GetBuffer());

		CSliderCtrl *Slider;
		Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF2));
		Slider->SetPos((1000000 - m_nSlider2));
		const int nTest = Slider->GetPos();
	}
	CATCH_IGNORE_EXCEPTIONS
}

void CTwoControl::OnNMReleasedcaptureSliderf2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF2));
	const int nCurrent = Slider->GetPos();
	if (nCurrent >= m_nSlider1)
		Slider->SetPos(m_nSlider1);

}

void CTwoControl::OnNMReleasedcaptureSliderf1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CSliderCtrl *Slider;
	Slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDERF2));
	const int nCurrent = Slider->GetPos();
	if (nCurrent >= m_nSlider1)
		Slider->SetPos(m_nSlider1);
}

void CTwoControl::OnEnChangeStep()
{
	TRY_EXCEPTIONS
	{
		CEdit * pEdit = reinterpret_cast< CEdit * >(GetDlgItem(IDC_STEP));
		CString sText;
		pEdit->GetWindowText(sText);
		m_nStep = boost::lexical_cast< int > (sText.GetBuffer());
	}
	CATCH_IGNORE_EXCEPTIONS
}

const int CTwoControl::GetLowerFrequency() const
{
	TRY_EXCEPTIONS
	{
		CEdit * pEdit = reinterpret_cast< CEdit * >(GetDlgItem(IDC_INDICATOR_F1));
		CString sText;
		pEdit->GetWindowText(sText);
		return boost::lexical_cast< int > (sText.GetBuffer());
	}
	catch(std::exception&)
	{
		return 1;
	}
}

const int CTwoControl::GetUpperFrequency() const
{
	TRY_EXCEPTIONS
	{
		CEdit * pEdit = reinterpret_cast< CEdit * >(GetDlgItem(IDC_INDICATOR_F2));
		CString sText;
		pEdit->GetWindowText(sText);
		return boost::lexical_cast< int > (sText.GetBuffer());
	}
	catch(std::exception&)
	{
		return 1;
	}
}

const int CTwoControl::GetStep() const
{
	TRY_EXCEPTIONS
	{
		CEdit * pEdit = reinterpret_cast< CEdit * >(GetDlgItem(IDC_STEP));
		CString sText;
		pEdit->GetWindowText(sText);
		return boost::lexical_cast< int > (sText.GetBuffer());
	}
	catch(std::exception&)
	{
		return 1;
	}
}