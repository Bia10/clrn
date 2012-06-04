// BulkLoopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BulkLoop.h"
#include "BulkLoopDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/*Summary
Fills buf with data per the selected method
*/
void stuffBuff(PUCHAR buf, LONG len, LONG seed, int method) {

	DWORD *dwBuf = (DWORD *) buf;
	srand((UINT)seed);

	int cnt = (method == 3) ? len / 4 : len;
	for (int i=0; i<cnt; i++) 
		switch (method) {
		case 0:
			buf[i] = (CHAR) seed;
			break;
		case 1:
			buf[i] = rand();
			break;
		case 2:
			buf[i] = (UCHAR)seed + i;
			break;
		case 3:
			dwBuf[i] = seed + i;
			break;
		}

}



/*Summary
The looping Bulk Xfer Thread function

The looping xfers need to run in a separate thread so as to be stoppable 
via a click on the "Stop" button.  Otherwise, that click will never get 
serviced and the app will loop indefinitely.

The "Start" button on click handler just calls this routine, passing a pointer
to the dialog (main window) object.  That way, this routine has access to all
the public members of the CBulkLoopDlg class.
*/

UINT XferLoop( LPVOID params ) {

    OVERLAPPED outOvLap, inOvLap; 

    CBulkLoopDlg *dlg = (CBulkLoopDlg *) params;
	char s[24];						ZeroMemory(s, 24);
	
	dlg->m_XferSize.GetLine(0,s,8);
	LONG xfer = atol(s);
	PUCHAR data = new UCHAR[xfer];	ZeroMemory(data,xfer);
	PUCHAR inData = new UCHAR[xfer];	ZeroMemory(inData,xfer);

    outOvLap.hEvent  = CreateEvent(NULL, false, false, "CYUSB_OUT"); 
    inOvLap.hEvent   = CreateEvent(NULL, false, false, "CYUSB_IN"); 

	dlg->m_SeedValue.GetLine(0,s,8);
	LONG seed = atol(s);

	int stopOnError = dlg->m_StopOnErrorChkBox.GetCheck();

	stuffBuff(data,xfer,seed,dlg->m_FillPatternComBox.GetCurSel());

	dlg->m_StatusLabel.SetWindowText(" STATUS: Transferring data . . .");
	dlg->m_SuccessCount.SetWindowText("0");
	dlg->m_FailureCount.SetWindowText("0");

	bool success;
	LONG nSuccess = 0;
	LONG nFailure = 0;

    if (dlg->m_DisableTimeoutChkBox.GetCheck())
    {
        dlg->OutEndpt->TimeOut = 0;
	    dlg->InEndpt->TimeOut = 0;
    }
    else
    {
        dlg->OutEndpt->TimeOut = 2000;
	    dlg->InEndpt->TimeOut = 2000;
    }

	for (;dlg->bLooping;) {
        LONG outlen,inlen,len;

        outlen = inlen = len = xfer;     // Use temp var because XferData can change the value of len

	    UCHAR  *outContext = dlg->OutEndpt->BeginDataXfer(data,outlen,&outOvLap);
	    UCHAR  *inContext = dlg->InEndpt->BeginDataXfer(inData,inlen,&inOvLap);

        dlg->OutEndpt->WaitForXfer(&outOvLap,2000); 
        dlg->InEndpt->WaitForXfer(&inOvLap,2000); 

        success = dlg->OutEndpt->FinishDataXfer(data, outlen, &outOvLap,outContext); 
        success = dlg->InEndpt->FinishDataXfer(inData,inlen, &inOvLap,inContext); 


		if (success) {
			bool pass = (memcmp(data,inData,len) == 0);
			if (pass)
				nSuccess++;
			else
				nFailure++;
		} else 
			nFailure++;

		sprintf(s,"%d",nSuccess);
		dlg->m_SuccessCount.SetWindowText(s);
		sprintf(s,"%d",nFailure);
		dlg->m_FailureCount.SetWindowText(s);

		if ((!success) && stopOnError) dlg->bLooping = false;
	} 

    CloseHandle(outOvLap.hEvent); 
    CloseHandle(inOvLap.hEvent); 

	delete [] data;
	delete [] inData;

	dlg->m_StatusLabel.SetWindowText(" STATUS: Stopped");

	dlg->XferThread = NULL;

    dlg->USBDevice->Close();

	return true;
}


/*Summary
CBulkLoopDlg dialog
*/
CBulkLoopDlg::CBulkLoopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBulkLoopDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBulkLoopDlg)
	m_DataValueRadioBtns = 0;
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CYICON);

}

void CBulkLoopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBulkLoopDlg)
	DDX_Control(pDX, IDC_DISABLE_TIMEOUT, m_DisableTimeoutChkBox);
	DDX_Control(pDX, IDC_TEST_BTN, m_TestBtn);
	DDX_Control(pDX, IDC_RESET_BTN, m_RefreshBtn);
	DDX_Control(pDX, IDC_DEVICELIST_COMBOX, m_DeviceListComBox);
	DDX_Control(pDX, IDC_START_BTN, m_StartBtn);
	DDX_Control(pDX, IDC_STATUS_LABEL, m_StatusLabel);
	DDX_Control(pDX, IDC_FILLPATTERN_COMBOX, m_FillPatternComBox);
	DDX_Control(pDX, IDC_STOP_ON_ERROR_CHKBOX, m_StopOnErrorChkBox);
	DDX_Control(pDX, IDC_SEED_EDIT, m_SeedValue);
	DDX_Control(pDX, IDC_FAILURE_LABEL, m_FailureCount);
	DDX_Control(pDX, IDC_SUCCESS_LABEL, m_SuccessCount);
	DDX_Control(pDX, IDC_XFERSIZE_EDIT, m_XferSize);
	DDX_Control(pDX, IDC_IN_COMBOX, m_InEndptComBox);
	DDX_Control(pDX, IDC_OUT_COMBOX, m_OutEndptComBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBulkLoopDlg, CDialog)
	//{{AFX_MSG_MAP(CBulkLoopDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_BTN, OnStartBtn)
	ON_CBN_SELCHANGE(IDC_OUT_COMBOX, OnSelchangeOutCombox)
	ON_CBN_SELCHANGE(IDC_IN_COMBOX, OnSelchangeInCombox)
	ON_BN_CLICKED(IDC_RESET_BTN, OnResetBtn)
	ON_BN_CLICKED(IDC_REFRESH_BTN, OnRefreshBtn)
	ON_BN_CLICKED(IDC_TEST_BTN, OnTestBtn)
	ON_CBN_SELCHANGE(IDC_DEVICELIST_COMBOX, OnSelchangeDeviceListCombox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*Summary
CBulkLoopDlg message handlers
*/
BOOL CBulkLoopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	XferThread = NULL;

	USBDevice = new CCyUSBDevice(m_hWnd);   // Create an instance of CCyUSBDevice

    OnRefreshBtn();

	m_XferSize.SetWindowText("1024");
	m_SeedValue.SetWindowText("2");
	m_FillPatternComBox.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBulkLoopDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);	
}




/*Summary
If you add a minimize button to your dialog, you will need the code below
to draw the icon.  For MFC applications using the document/view model,
this is automatically done for you by the framework.
*/
void CBulkLoopDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


/*Summary
The system calls this to obtain the cursor to display while the user drags
the minimized window.
*/
HCURSOR CBulkLoopDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//___________________________________________________________________________


/*Summary
Executes on start button click. Gets the end points of the device and starts the thread xferloop
*/
void CBulkLoopDlg::OnStartBtn() 
{
    if (XferThread) {
        bLooping = false;

        //		InEndpt->Abort();
        //		InEndpt->Reset();

        //		OutEndpt->Abort();
        //		OutEndpt->Reset();

        m_StartBtn.SetWindowText("Start");
    } else {

        char s[12];
        ZeroMemory(s,12);
        m_XferSize.GetLine(0,s,8);
        LONG xfer = atol(s);
        if (xfer > 2048) {
            xfer = 2048;
            m_XferSize.SetWindowText("2048");
        }

        int n = USBDevice->DeviceCount();

		int i = m_DeviceListComBox.GetCurSel();
		if(i==0xffffffff)// for no selection
			return;

        if (n != 0)
        {
            USBDevice->Open(DeviceIndex);
            OutEndpt = USBDevice->EndPoints[m_OutEndptComBox.GetItemData(m_OutEndptComBox.GetCurSel())];
            InEndpt = USBDevice->EndPoints[m_InEndptComBox.GetItemData(m_InEndptComBox.GetCurSel())];

            // Launch the looping thread  (Calls XferLoop() function, above.)
            if (xfer && USBDevice->IsOpen()) {
                bLooping = true;
                XferThread = AfxBeginThread(XferLoop, this);
            }

            m_StartBtn.SetWindowText("Stop");
        }
    }

}

//___________________________________________________________________________

void CBulkLoopDlg::OnSelchangeOutCombox() 
{
//	OutEndpt = (CCyUSBEndPoint *) m_OutEndptComBox.GetItemDataPtr(m_OutEndptComBox.GetCurSel());
}

//___________________________________________________________________________

void CBulkLoopDlg::OnSelchangeInCombox() 
{
//	InEndpt = (CCyUSBEndPoint *) m_InEndptComBox.GetItemDataPtr(m_InEndptComBox.GetCurSel());
}

//___________________________________________________________________________




/*Summary
Executes on selecting the device list combox. Selecting the end point.
*/
void CBulkLoopDlg::OnSelchangeDeviceListCombox() 
{
    int i;
    char s[12];

    //DeviceIndex = m_DeviceListComBox.GetCurSel();

    USBDevice->Open(DeviceIndex);

	int epts = USBDevice->EndPointCount();
	
    CCyUSBEndPoint *endpt;

	// Load the endpoint combo boxes
    // Skip endpoint 0, which we know is the Control Endpoint
	for (i=1; i<epts; i++)
    {    
        endpt = USBDevice->EndPoints[i];

        if (endpt->Attributes == 2)    // Bulk
        {
            sprintf(s, "0x%02X", endpt->Address);

            if (endpt->Address & 0x80)
            {
                m_InEndptComBox.AddString(s);
                m_InEndptComBox.SetItemData(m_InEndptComBox.GetCount()-1,i);
            }
            else
            {
                m_OutEndptComBox.AddString(s);
                m_OutEndptComBox.SetItemData(m_OutEndptComBox.GetCount()-1,i);
            }
        }
    }

    m_InEndptComBox.SetCurSel(0);
	m_OutEndptComBox.SetCurSel(0);

    OnSelchangeInCombox();		// Set the InEndpt member
	OnSelchangeOutCombox();		// Set the OutEndpt member

}




/*Summary
Executes on Reset button click. Resets the USB device, releases the resources allocated.
*/
void CBulkLoopDlg::OnResetBtn() 
{
    if (XferThread) OnStartBtn();  // Hit the "Stop" button, resetting endpoints
	
	USBDevice->Reset();	

	delete USBDevice;
	
	// Clear the combo boxes
	m_InEndptComBox.ResetContent();  
	m_OutEndptComBox.ResetContent();

	// Temporarily save the Xfer size and Seed values
	char s1[12];
	char s2[12];
	m_XferSize.GetWindowText(s1,8);
	m_SeedValue.GetWindowText(s2,8);
	int fillmethod = m_FillPatternComBox.GetCurSel();

	// Re-initialize the combo boxes, USBDevice, etc
	OnInitDialog();

	// Restore the previous values for these controls
	m_XferSize.SetWindowText(s1);
	m_SeedValue.SetWindowText(s2);
	m_FillPatternComBox.SetCurSel(fillmethod);

}



/*Summary
Executes on Refresh button click. Stops the thread, if it is running, aborts and resets the  endpoint.
Refresh the Checks for all the devices connected.
*/
void CBulkLoopDlg::OnRefreshBtn() 
{
    int i;

    // if we are currently running loopback, stop
    if (XferThread)
    {
        bLooping = false;

        InEndpt->Abort();
        InEndpt->Reset();

        OutEndpt->Abort();
        OutEndpt->Reset();

        m_StartBtn.SetWindowText("Start");
    }

    // clear the combo boxes
    m_DeviceListComBox.ResetContent();
	m_InEndptComBox.ResetContent();  
	m_OutEndptComBox.ResetContent();

    // make sure there is at lesat one device out there
    if (USBDevice->DeviceCount())
    {
        // search for all connected devices, and add them to the device
        // combo box
        for (i = 0; i < USBDevice->DeviceCount(); i++)
        {
			USBDevice->Open(i);

			//Only display the Bulkloop back compatinle device, other device will not be displayed
			//Get config descriptor
			USB_CONFIGURATION_DESCRIPTOR ConfDesc;
			USBDevice->GetConfigDescriptor(&ConfDesc);
			
			if(ConfDesc.bNumInterfaces==1)
			{// Number of interface one
				//Get Interface descriptor
				USB_INTERFACE_DESCRIPTOR IntfDesc;
				USBDevice->GetIntfcDescriptor(&IntfDesc);
				if(IntfDesc.bAlternateSetting==0)
				{//
					if(IntfDesc.bNumEndpoints==4)
					{// Number of endpoint is four
						m_DeviceListComBox.AddString(USBDevice->DeviceName);
						DeviceIndex = i;
						return;
					}
				}				
			}

            
        }


        m_DeviceListComBox.SetCurSel(0);
        OnSelchangeDeviceListCombox();		// This will set the device index and load the ep lists

    }	
}


void CBulkLoopDlg::OnTestBtn() 
{
  

	
}


