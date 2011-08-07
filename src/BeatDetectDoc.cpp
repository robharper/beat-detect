// BeatDetectDoc.cpp : implementation of the CBeatDetectDoc class
//

#include "stdafx.h"
#include "BeatDetect.h"

#include "BeatDetectDoc.h"
#include "DSP.h"
#include "BDOnsetStage.h"
#include "BDRealTimeStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBeatDetectDoc

IMPLEMENT_DYNCREATE(CBeatDetectDoc, CDocument)

BEGIN_MESSAGE_MAP(CBeatDetectDoc, CDocument)
	//{{AFX_MSG_MAP(CBeatDetectDoc)
	ON_COMMAND(ID_FILE_SAVE_ONSETS, OnSaveOnsets)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectDoc construction/destruction

CBeatDetectDoc::CBeatDetectDoc()
{
	// TODO: add one-time construction code here

}

CBeatDetectDoc::~CBeatDetectDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CBeatDetectDoc diagnostics

#ifdef _DEBUG
void CBeatDetectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBeatDetectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBeatDetectDoc commands

BOOL CBeatDetectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

    // Release all streams
	m_ASInput.ReleaseData();
    for( INT32 ii=0; ii<NUM_BANDS; ii++ )
        m_ASInputBands[ii].ReleaseData();
    m_StrmBeatOutput.ReleaseData();
    m_StrmBeatPeriod.ReleaseData();
    m_StrmBeatTempo.ReleaseData();
    m_StrmBeatInfo.ReleaseData();
    m_StrmOnsetOutput.ReleaseData();
    m_StrmOnsetInternal.ReleaseData();
    //
    for( INT32 ix=0; ix<6; ix++ )
        m_ASInputBands[ix].ReleaseData();

    InitializeSettings();

	return TRUE;
}

BOOL CBeatDetectDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

    CDialog DlgWorking;
    DlgWorking.Create( IDD_WORKING );
    DlgWorking.ShowWindow(SW_SHOW);
    DlgWorking.CenterWindow();
    DlgWorking.UpdateWindow();
    
	
    /////////////////////////////////////////////////////////////////////
    //
    // Beat Processing...
    //

	HRESULT hr = m_ASInput.LoadFromWaveFile( lpszPathName );
    if( FAILED(hr) )
        return hr==S_OK;
    hr = m_ASInput.Normalize();
    if( FAILED(hr) )
        return hr==S_OK;
    
    ASSERT( m_ASInput.GetSampleRate() == 44100 );

    // Stage 1: Onset
    CBDOnsetStage Stage1;
    hr = Stage1.CreateOnsetStream( &m_ASInput, &m_StrmOnsetOutput, &m_StrmOnsetInternal );
    if( FAILED(hr) )
        return hr==S_OK;

    // Stage 2: RealTime Stage
    CBDRealTimeStage Stage2;
    hr = Stage2.CreateBeatStream( &m_StrmOnsetOutput, &m_StrmBeatOutput,
                                  &m_StrmBeatTempo,   &m_StrmBeatPeriod, &m_StrmBeatInfo );
    if( FAILED(hr) )
        return hr==S_OK;

    //
    // .. End Beat Processing
    //
    /////////////////////////////////////////////////////////////////////

    DlgWorking.CloseWindow();
    DlgWorking.DestroyWindow();


    // Automation - Save and Exit
    if( ((CBeatDetectApp *)AfxGetApp())->m_fAutomate )
    {
        // Save output
        CString strSaveName = lpszPathName;
        strSaveName.Insert( strSaveName.GetLength()-4, "_Output" );
        OnSaveDocument( strSaveName );
    }
	
	return hr==S_OK;
}

BOOL CBeatDetectDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
    ////////////////////////
	// Temporary Data Saver
    //m_ASOnsetLPF.SaveToWaveFile( lpszPathName );

    CAudioStream StrmClick, StrmRendered;
    
    HRESULT hr = RenderClickTrack( &m_StrmBeatOutput, &StrmClick );
    if( FAILED(hr) )
        return FALSE;
    
    hr = CDSP::Mix( &StrmClick, 1, &m_ASInput, 0.75, &StrmRendered );
    if( FAILED(hr) )
        return FALSE;

    hr = StrmRendered.DeNormalize( sizeof(signed short)*8 );
    if( FAILED(hr) )
        return FALSE;

    hr = StrmRendered.SaveToWaveFile( lpszPathName );
    if( FAILED(hr) )
        return FALSE;
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////
HRESULT CBeatDetectDoc::RenderClickTrack
(
    CDataStream *pStrmIn, 
    CAudioStream *pStrmOut
)
{
    HRESULT hr = S_OK;

    DOUBLE dDecFactor = 44100/pStrmIn->GetSampleRate();
    
    // Create click track at 44100Hz, 16bits, with correct number of samples
    hr = pStrmOut->CreateData( sizeof(FLOAT)*8, 44100, (INT32)(pStrmIn->GetNumSamples()*dDecFactor), TRUE );
    if( FAILED(hr) )
        return hr;

    // Load click sound
    CAudioStream StrmClick;
    hr = StrmClick.LoadFromWaveFile( _T("click.wav") );
    if( FAILED(hr) )
        return hr;
    hr = StrmClick.Normalize();
    if( FAILED(hr) )
        return hr;

    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();
    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    FLOAT * pflDataClick = (FLOAT *)StrmClick.GetData();

    DOUBLE dOutSam = 0;
    BOOL fClick = FALSE;
    for( INT32 iInSam = 0; iInSam < pStrmIn->GetNumSamples(); iInSam++ )
    {
        if( (pflDataIn[iInSam] > 0) && !fClick )
        {
            // Found a click, render it
            INT32 nLen = min( StrmClick.GetNumSamples(), pStrmOut->GetNumSamples()-(INT32)dOutSam-1 );
            memcpy( &(pflDataOut[(INT32)dOutSam]), pflDataClick, sizeof(FLOAT) * nLen );
            fClick = TRUE;
        }
        else
        {
            // No click here, fill with zeros
            memset( &(pflDataOut[(INT32)dOutSam]), 0, sizeof(FLOAT) * 
                    ((INT32)(dOutSam+dDecFactor) - (INT32)dOutSam) );
            if(pflDataIn[iInSam] == 0)
                fClick = FALSE;
        }
        dOutSam += dDecFactor;
    }

    return hr;
}

void CBeatDetectDoc::OnSaveOnsets() 
{
	////////////////////////
	// Temporary Data Saver
    //m_ASOnsetLPF.SaveToWaveFile( lpszPathName );

    CString newName;
    CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

    if (!AfxGetApp()->DoPromptFileName(newName,
		  AFX_IDS_SAVEFILE, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return;       // don't even attempt to save

    CAudioStream StrmClick, StrmRendered;
    
    HRESULT hr = RenderClickTrack( &m_StrmOnsetOutput, &StrmClick );
    if( FAILED(hr) )
        return;
   
    if( 1 )
    {
        hr = CDSP::Mix( &StrmClick, 1, &m_ASInput, 0.75, &StrmRendered );
        if( FAILED(hr) )
            return;

        hr = StrmRendered.DeNormalize( sizeof(signed short)*8 );
        if( FAILED(hr) )
            return;

        hr = StrmRendered.SaveToWaveFile( newName );
        if( FAILED(hr) )
            return;
    }
    else
    {
        hr = StrmClick.DeNormalize( sizeof(signed short)*8 );
        if( FAILED(hr) )
            return;

        hr = StrmClick.SaveToWaveFile( newName );
        if( FAILED(hr) )
            return;
    }

	return;	
}
