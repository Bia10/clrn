// src/RangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CypressGUI.h"
#include "RangeDlg.h"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


// CRangeDlg dialog

IMPLEMENT_DYNAMIC(CRangeDlg, CDialog)

CRangeDlg::CRangeDlg(		
					 int& nLowerFrequency,
					 int& nUpperFrequency,
					 int& nStep,
					 TUpdateFun& UpdateFun,
					 CWnd* pParent
					 ) : 
	CDialog(CRangeDlg::IDD, pParent),
	m_nLowerFrequency(nLowerFrequency),
	m_nUpperFrequency(nUpperFrequency),
	m_nStep(nStep),
	m_funUpdate(UpdateFun)
{
	Create(CRangeDlg::IDD, pParent);
}

CRangeDlg::~CRangeDlg()
{
}

void CRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RANGES, m_RangesCtrl);
}


BEGIN_MESSAGE_MAP(CRangeDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BTN, &CRangeDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &CRangeDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDOK, &CRangeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRangeDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDDEFAULT, &CRangeDlg::OnBnClickedDefault)
END_MESSAGE_MAP()


void CRangeDlg::Init(TCallBackFun& CallBackFun)
{
	m_funCallBack = CallBackFun;
	m_RangesCtrl.ResetContent();
	Add();
}

void CRangeDlg::Add()
{
	m_funUpdate();

	_tstring sText = _T("Start: ");
	sText += boost::lexical_cast< _tstring > (m_nLowerFrequency);
	if (m_nUpperFrequency)
	{
		sText += _T(" End: ");
		sText += boost::lexical_cast< _tstring > (m_nUpperFrequency);
	}
	if (m_nStep)
	{
		sText += _T(" Step: ");
		sText += boost::lexical_cast< _tstring > (m_nStep);
	}

	const int nIndex = m_RangesCtrl.FindString(0, sText.c_str());
	if (nIndex < 0)
		m_RangesCtrl.AddString(sText.c_str());
}

void CRangeDlg::Remove()
{
	const int nIndex = m_RangesCtrl.GetCurSel();
	if (nIndex < 0)
		return;
	m_RangesCtrl.DeleteString(nIndex);
}


void CRangeDlg::OnBnClickedAddBtn()
{
	Add();
}

void CRangeDlg::OnBnClickedRemoveBtn()
{
	Remove();
}

void CRangeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();

	std::vector< CRange > Ranges;
	const int nSize = m_RangesCtrl.GetCount();
	for (int i = 0; i < nSize; ++i)
	{
		CString sTmp;
		m_RangesCtrl.GetText(i, sTmp);
		const _tstring sCurrent(sTmp.GetBuffer());

		CRange Current;

		if (m_nUpperFrequency && m_nStep)
		{
			const _tstring::size_type nStart = sCurrent.find(_T("Start: "));
			const _tstring::size_type nEnd = sCurrent.find(_T(" End: "));
			const _tstring::size_type nStep = sCurrent.find(_T(" Step: "));

			const _tstring sStart(&sCurrent[nStart + 7], &sCurrent[nEnd]);
			const _tstring sEnd(&sCurrent[nEnd + 6], &sCurrent[nStep]);
			const _tstring sStep(&sCurrent[nStep + 7]);

			Current.Lower	= boost::lexical_cast< int > (sStart);
			Current.Upper	= boost::lexical_cast< int > (sEnd);
			Current.Step	= boost::lexical_cast< int > (sStep);
		}
		else
		{
			const _tstring sStart(&sCurrent[7]);
			Current.Lower	= boost::lexical_cast< int > (sStart);
			Current.Upper	= 0;
			Current.Step	= 0;	
		}

		Ranges.push_back(Current);
	}
	
	m_funCallBack(Ranges);
}

void CRangeDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CRangeDlg::OnBnClickedDefault()
{
	m_RangesCtrl.ResetContent();

	for (int i = 1; i < 100000; i *= 10)
	{
		_tstring sText = (_tformat(_T("Start: %s End: %s Step: %s")) % boost::lexical_cast< _tstring > (i) % boost::lexical_cast< _tstring > (i * 10) % boost::lexical_cast< _tstring > (i)).str();
		m_RangesCtrl.AddString(sText.c_str());
	}
}
