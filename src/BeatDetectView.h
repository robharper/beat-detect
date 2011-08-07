// BeatDetectView.h : interface of the CBeatDetectView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEATDETECTVIEW_H__AD42618C_0BD5_4F80_890B_50A730960CD3__INCLUDED_)
#define AFX_BEATDETECTVIEW_H__AD42618C_0BD5_4F80_890B_50A730960CD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxtempl.h>


class CBeatDetectView : public CScrollView
{
protected: // create from serialization only
	CBeatDetectView();
	DECLARE_DYNCREATE(CBeatDetectView)

// Attributes
public:
	CBeatDetectDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeatDetectView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBeatDetectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
    DOUBLE          m_dZoom;       // Seconds per pixel
    BOOL            m_fScrollError;


// Generated message map functions
protected:
	void DrawStream( CDC * pDC, CDataStream* pStrm, LPRECT lprc );
	void GetViewRect( RECT * prc );
	HRESULT CreateStreamsList( CList<CDataStream*,CDataStream*> * pList );

    void DrawTimeRuler( CDC * pDC, LPRECT lprc );

	//{{AFX_MSG(CBeatDetectView)
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewStreamChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BeatDetectView.cpp
inline CBeatDetectDoc* CBeatDetectView::GetDocument()
   { return (CBeatDetectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEATDETECTVIEW_H__AD42618C_0BD5_4F80_890B_50A730960CD3__INCLUDED_)
