// BeatDetectDoc.h : interface of the CBeatDetectDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BEATDETECTDOC_H__5E07C1E3_D4EF_4224_8B42_71A128849542__INCLUDED_)
#define AFX_BEATDETECTDOC_H__5E07C1E3_D4EF_4224_8B42_71A128849542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBeatDetectDoc : public CDocument
{
protected: // create from serialization only
	CBeatDetectDoc();
	DECLARE_DYNCREATE(CBeatDetectDoc)

// Attributes
public:
    CAudioStream    m_ASInput;
    CAudioStream    m_ASInputBands[6];
    
    // Onset Detect Streams
    CDataStream     m_StrmOnsetOutput;
    CDataStream     m_StrmOnsetInternal;

    // Beat Detect Streams
    CDataStream     m_StrmBeatOutput;
    CDataStream     m_StrmBeatTempo;
    CDataStream     m_StrmBeatPeriod;

    CDataStream     m_StrmBeatInfo;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeatDetectDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBeatDetectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generated message map functions
protected:
	HRESULT RenderClickTrack( CDataStream *pStrmIn, CAudioStream *pStrmClk );
	//{{AFX_MSG(CBeatDetectDoc)
	afx_msg void OnSaveOnsets();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEATDETECTDOC_H__5E07C1E3_D4EF_4224_8B42_71A128849542__INCLUDED_)
