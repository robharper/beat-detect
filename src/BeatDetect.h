// BeatDetect.h : main header file for the BEATDETECT application
//

#if !defined(AFX_BEATDETECT_H__5616A048_D876_40C7_9D75_EC831A939C9B__INCLUDED_)
#define AFX_BEATDETECT_H__5616A048_D876_40C7_9D75_EC831A939C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectApp:
// See BeatDetect.cpp for the implementation of this class
//

class CBeatDetectApp : public CWinApp
{
public:
	CBeatDetectApp();

    BOOL    m_fAutomate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeatDetectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBeatDetectApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEATDETECT_H__5616A048_D876_40C7_9D75_EC831A939C9B__INCLUDED_)
