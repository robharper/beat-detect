// BeatDetectView.cpp : implementation of the CBeatDetectView class
//

#include "stdafx.h"
#include "BeatDetect.h"
#include "MainFrm.h"

#include "BeatDetectDoc.h"
#include "BeatDetectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RULER_HEIGHT        20

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectView

IMPLEMENT_DYNCREATE(CBeatDetectView, CScrollView)

BEGIN_MESSAGE_MAP(CBeatDetectView, CScrollView)
	//{{AFX_MSG_MAP(CBeatDetectView)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_BN_CLICKED(IDC_VIEW_INPUT_CHECK, OnViewStreamChange)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_VIEW_ONSET_CHECK, OnViewStreamChange)
	ON_BN_CLICKED(IDC_VIEW_ONSET2_CHECK, OnViewStreamChange)
    ON_BN_CLICKED(IDC_VIEW_BEATOUT_CHECK, OnViewStreamChange)
    ON_BN_CLICKED(IDC_VIEW_TEMPOOUT_CHECK, OnViewStreamChange)
    ON_BN_CLICKED(IDC_VIEW_PERIODOUT_CHECK, OnViewStreamChange)
    ON_BN_CLICKED(IDC_VIEW_INFOOUT_CHECK, OnViewStreamChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectView construction/destruction

CBeatDetectView::CBeatDetectView()  : m_dZoom(0.016)
{
	// TODO: add construction code here

}

CBeatDetectView::~CBeatDetectView()
{
}

BOOL CBeatDetectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectView drawing

void CBeatDetectView::OnDraw(CDC* pDC)
{
	CBeatDetectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    RECT rcView;
    GetViewRect( &rcView );

    CList<CDataStream*,CDataStream*> lstStream;
    HRESULT hr = CreateStreamsList( &lstStream );
    if( S_OK == hr )
    {
        //////////////////
        // Draw Time Ruler
        RECT rcRuler = rcView;
        rcRuler.bottom = rcRuler.top + RULER_HEIGHT;
        rcView.top = rcRuler.bottom;
        DrawTimeRuler( pDC, &rcRuler );
	    
        //////////////////
        // Draw Data Streams
    
        // Determine rects for drawing streams
        INT32 nStreams = lstStream.GetCount();
        
        INT32 nStreamHeight = rcView.bottom/nStreams;
        RECT rcStream = rcView;
        rcStream.bottom = rcStream.top + nStreamHeight;

        POSITION pos = lstStream.GetHeadPosition();
        while( pos != NULL )
        {
            CDataStream* pStrm = lstStream.GetNext(pos);
            // Draw
            DrawStream( pDC, pStrm, &rcStream );
            // Offset stream rect
            OffsetRect( &rcStream, 0, nStreamHeight );
        } 
    }
    //////////////////
}

void CBeatDetectView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

    CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
    CDialogBar* pDlgBar  = pMainFrm->GetDialogBar();

    ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_INPUT_CHECK))->SetCheck(TRUE);
    ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_ONSET_CHECK))->SetCheck(TRUE);
    ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_BEATOUT_CHECK))->SetCheck(TRUE);
    ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_TEMPOOUT_CHECK))->SetCheck(TRUE);
    ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_PERIODOUT_CHECK))->SetCheck(TRUE);

	OnViewStreamChange();
}

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectView diagnostics

#ifdef _DEBUG
void CBeatDetectView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBeatDetectView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CBeatDetectDoc* CBeatDetectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBeatDetectDoc)));
	return (CBeatDetectDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectView message handlers

void CBeatDetectView::OnViewZoomIn() 
{
    // Messy crap, but this is not a pro app...
    if( m_dZoom > 0.0001 )
    {
	    m_dZoom = m_dZoom / 2;
        OnViewStreamChange();
    }
}

void CBeatDetectView::OnViewZoomOut() 
{
    // Messy crap, but this is not a pro app...
	if( m_dZoom < 0.1 )
    {
	    m_dZoom = m_dZoom * 2;
        OnViewStreamChange();
    }	
}


HRESULT CBeatDetectView::CreateStreamsList
(
    CList<CDataStream*,CDataStream*> *pList
)
{
    CBeatDetectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    if( NULL == pList )
        return E_INVALIDARG;

    CMainFrame* pMainFrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
    CDialogBar* pDlgBar  = pMainFrm->GetDialogBar();

    // Clear out list
    while( !pList->IsEmpty() )
        pList->RemoveTail();
    
    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_INPUT_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_ASInput );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_ONSET_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmOnsetOutput );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_ONSET2_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmOnsetInternal );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_BEATOUT_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmBeatOutput );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_TEMPOOUT_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmBeatTempo );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_PERIODOUT_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmBeatPeriod );
    }

    if( ((CButton*)pDlgBar->GetDlgItem(IDC_VIEW_INFOOUT_CHECK))->GetCheck() )
    {
        pList->AddTail( &pDoc->m_StrmBeatInfo );
    }

    if( !pList->IsEmpty() )
        return S_OK;
    else
        return S_FALSE;
}


void CBeatDetectView::OnViewStreamChange() 
{
	// Update the view with changes in the streams we are to show
    CList<CDataStream*,CDataStream*> lstStream;

    DOUBLE dDuration = 0;

    HRESULT hr = CreateStreamsList( &lstStream );
    if( S_OK == hr )
    {
        POSITION pos = lstStream.GetHeadPosition();
        while( pos != NULL )
        {
            CDataStream* pStrm = lstStream.GetNext(pos);
            if( pStrm->GetDuration() > dDuration )
                dDuration = pStrm->GetDuration();
        }        
    }
    // Set View Size
    RECT rcWnd;
    GetClientRect(&rcWnd);
    
    SIZE sz;
    sz.cx = max((INT32)(dDuration/m_dZoom), rcWnd.right-rcWnd.left);
    sz.cy = rcWnd.bottom-rcWnd.top;
    SetScrollSizes(MM_TEXT, sz);

    Invalidate( TRUE );

    ////////////////////
    // Scroll Error Crap
    // Scrollbars suck and can't go higher than 32767, so warn if this happens
    m_fScrollError = (sz.cx > 32767);

    return;
}


void CBeatDetectView::OnSize
(
    UINT nType, 
    int cx, 
    int cy
) 
{
	CScrollView::OnSize(nType, cx, cy);
	
    //////////
    // Make vertical dimension equal to client size...   
    SIZE szTotal;

    RECT rcView;
    GetViewRect(&rcView);
    szTotal.cx = rcView.right;

    RECT rcWnd;
    GetClientRect(&rcWnd);
    szTotal.cy = rcWnd.bottom-rcWnd.top;
    
    SetScrollSizes(MM_TEXT, szTotal);
    //////////
}


void CBeatDetectView::GetViewRect
(
    RECT *prc
)
{
    INT32 nTemp;
    SIZE szTotal, szTemp1, szTemp2;
	GetDeviceScrollSizes( nTemp, szTotal, szTemp1, szTemp2 );

    prc->top = 0;
    prc->left = 0;
    prc->bottom = szTotal.cy;
    prc->right = szTotal.cx;
}


void CBeatDetectView::DrawStream
(
    CDC * pDC,
    CDataStream *pStrm, 
    LPRECT lprc
)
{
    INT32 nCentreLine = (lprc->top + lprc->bottom)/2;


    // Draw horizontal line at zero
    CBrush Brush;
    Brush.CreateSolidBrush( 0x0000FF00 );
    RECT rcDraw = *lprc;
    rcDraw.top    = nCentreLine;
    rcDraw.bottom = nCentreLine + 1;
    pDC->FillRect( &rcDraw, &Brush );
    Brush.DeleteObject();


    // Hurt Casting! ***************************************************************
    // Assume Float
    FLOAT* pData = (FLOAT*)pStrm->GetData();

    RECT rcClip;
    pDC->GetClipBox( &rcClip );
    InflateRect( &rcClip, 2, 0 );

    // Create Draw Brush - Blue
    if( m_fScrollError )
        Brush.CreateSolidBrush( 0x000000FF );
    else
        Brush.CreateSolidBrush( 0x00CC0000 );

    CBrush BrushSpike;
    BrushSpike.CreateSolidBrush( 0x00009900 );

    // Calculate draw region
    INT32 nSampleScale = (INT32)((lprc->bottom - lprc->top)/2);

    // Calculate Sample Offsets
    INT32 nStartSample = max((INT32)((DOUBLE)rcClip.left*m_dZoom*pStrm->GetSampleRate()), 0);
    INT32 nEndSample = min((INT32)((DOUBLE)rcClip.right*m_dZoom*pStrm->GetSampleRate()), pStrm->GetNumSamples());

    DOUBLE dSamplesPerPixel = m_dZoom*pStrm->GetSampleRate();
    DOUBLE dSamCurPixel = nStartSample;
    DOUBLE dSamNextPixel = dSamCurPixel + dSamplesPerPixel;

    // Loop through all samples
    INT32 ix = rcClip.left;
    FLOAT flMin = 1;
    FLOAT flMax = -1;
    BOOL fNext = FALSE;
    for( INT32 iSam = nStartSample; iSam < nEndSample; iSam++ )
    {
        // Draw and go to the next pixel
        while( iSam > (INT32)dSamNextPixel )
        {
            // Draw this pixel
            RECT rcDraw;
            rcDraw.left   = ix;
            rcDraw.right  = ix+1;
            rcDraw.top    = nCentreLine - (INT32)(flMax*nSampleScale);
            rcDraw.bottom = nCentreLine - (INT32)(flMin*nSampleScale) + 1;

            if( flMax > 1 )
                pDC->FillRect( &rcDraw, &BrushSpike );
            else
                pDC->FillRect( &rcDraw, &Brush );

            // Next pixel
            dSamNextPixel += dSamplesPerPixel;
            ix += 1;
            fNext = TRUE;
        }
        if( fNext )
        {
            // Swap min and max to make continuous plot for next set of samples
            // Only do this after drawing all pixels for current samples because
            // we need to make sure to draw situations where samples/pixel is less
            // than one
            FLOAT flTemp = flMin;
            flMin = flMax;
            flMax = flTemp;
            fNext = FALSE;
        }
     
        if( pData[iSam] > flMax )
            flMax = pData[iSam];
        if( pData[iSam] < flMin )
            flMin = pData[iSam];
    }

    // Clean Up
    Brush.DeleteObject();
    BrushSpike.DeleteObject();
}


void CBeatDetectView::DrawTimeRuler
( 
    CDC * pDC, 
    LPRECT lprc
)
{
    // Draw Background
    CBrush Brush;
    Brush.CreateSolidBrush( 0x00BBBBBB );
    pDC->FillRect( lprc, &Brush );
    Brush.DeleteObject();

    // Draw horizontal marker lines
    Brush.CreateSolidBrush( 0x00000000 );

    RECT rcClip;
    pDC->GetClipBox( &rcClip );
    INT32 nStartPixel = rcClip.left;
    INT32 nEndPixel = rcClip.right+2;

    FLOAT flTicks = (FLOAT)(1/m_dZoom);
    INT32 nTickHeight = lprc->bottom - lprc->top - 1;
    while( flTicks > 25 )
    {
        FLOAT fli = (INT32)(nStartPixel/flTicks) * flTicks;
        for(; fli<nEndPixel; fli+=flTicks)
        {
            RECT rcTick;
            rcTick.bottom = lprc->bottom;
            rcTick.top = rcTick.bottom - nTickHeight;
            rcTick.left = (LONG)fli;
            rcTick.right = (LONG)fli+1;
            pDC->FillRect( &rcTick, &Brush );
        }
        nTickHeight = (nTickHeight * 2) / 3;
        flTicks /= 2;
    }
    
    Brush.DeleteObject();
}
