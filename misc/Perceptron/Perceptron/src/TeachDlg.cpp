// src/TeachDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Perceptron.h"
#include "inc/TeachDlg.h"


// CTeachDlg dialog

IMPLEMENT_DYNAMIC(CTeachDlg, CDialog)

CTeachDlg::CTeachDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachDlg::IDD, pParent)
{

	EnableAutomation();

}

CTeachDlg::~CTeachDlg()
{
}

void CTeachDlg::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CTeachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILEPATH, m_FilePath);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboTypeCtrl);
}


BEGIN_MESSAGE_MAP(CTeachDlg, CDialog)
	ON_BN_CLICKED(IDOPEN, &CTeachDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDOKTEACH, &CTeachDlg::OnBnClickedOkteach)
	ON_BN_CLICKED(IDCANCELTEACH, &CTeachDlg::OnBnClickedCancelteach)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTeachDlg, CDialog)
END_DISPATCH_MAP()

// Note: we add support for IID_ITeachDlg to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {C10E7977-2388-424A-A9F9-4969F5BCB883}
static const IID IID_ITeachDlg =
{ 0xC10E7977, 0x2388, 0x424A, { 0xA9, 0xF9, 0x49, 0x69, 0xF5, 0xBC, 0xB8, 0x83 } };

BEGIN_INTERFACE_MAP(CTeachDlg, CDialog)
	INTERFACE_PART(CTeachDlg, IID_ITeachDlg, Dispatch)
END_INTERFACE_MAP()


// CTeachDlg message handlers

void CTeachDlg::OnBnClickedOpen()
{
	// Получаем имя файла
	LPCTSTR strFilter = { L"BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, L".bmp", NULL, 0, strFilter, this);

	if( FileDlg.DoModal() == IDOK )
	{
		const _tstring sFullpath = _tstring(FileDlg.GetFolderPath()) + L"\\" + _tstring(FileDlg.GetFileName());

		m_sPath.assign(sFullpath.c_str());
		m_FilePath.SetWindowText(sFullpath.c_str());
	}
	else
		return;
}

void CTeachDlg::OnBnClickedOkteach()
{	
	CString sName;

	if (m_sPath.empty())
	{
		MessageBox(L"Please select image!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	const int nIndex = m_ComboTypeCtrl.GetCurSel();

	if (nIndex == -1)
	{
		MessageBox(L"Please enter image type!", L"Perceptron info", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	m_bType = nIndex == 0;
	OnOK();
}

void CTeachDlg::OnBnClickedCancelteach()
{
	m_sPath.clear();
	OnCancel();
}

const _tstring CTeachDlg::GetPath() const
{
	return m_sPath;
}


const bool CTeachDlg::GetType() const
{
	return m_bType;
}