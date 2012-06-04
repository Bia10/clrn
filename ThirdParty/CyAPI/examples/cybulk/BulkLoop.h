// BulkLoop.h : main header file for the BULKLOOP application
//

#if !defined(AFX_BULKLOOP_H__94303D2D_6EC6_4505_BE2A_85BDF0C7D94F__INCLUDED_)
#define AFX_BULKLOOP_H__94303D2D_6EC6_4505_BE2A_85BDF0C7D94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBulkLoopApp:
// See BulkLoop.cpp for the implementation of this class
//

class CBulkLoopApp : public CWinApp
{
public:
	CBulkLoopApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBulkLoopApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBulkLoopApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BULKLOOP_H__94303D2D_6EC6_4505_BE2A_85BDF0C7D94F__INCLUDED_)
