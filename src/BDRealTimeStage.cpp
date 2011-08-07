// BDRealTimeStage.cpp: implementation of the CBDRealTimeStage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDRealTimeStage.h"
#include "BDIOIStatCollector.h"
#include "BDNodeControl.h"
#include "BDNodeVarSampler.h"
#include "MFileWriter.h"
#include "BDUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define OUTPUT_ONSETS       0
#define OUTPUT_ACTUALBEATS  0
#define FIND_BEATS          1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDRealTimeStage::CBDRealTimeStage()
{

}

CBDRealTimeStage::~CBDRealTimeStage()
{

}


HRESULT CBDRealTimeStage::CreateBeatStream
( 
    CDataStream *pStrmIn,         // Onset stream, constant tempo
    CDataStream *pStrmOut,        // Beat steram, constant tempo
    CDataStream *pStrmTempo,      // Sampling period, constant tempo
    CDataStream *pStrmBeatPeriod,  // Winning loop period, constant tempo
    CDataStream *pStrmBeatInfo
)
{
    HRESULT hr = S_OK;

    /////////
    // Debug
#if OUTPUT_ONSETS
    CMFileWriter<FLOAT> MWriter;
    MWriter.Open( "Onsets.m", TRUE );
#elif OUTPUT_ACTUALBEATS
    CMFileWriter<FLOAT> MWriter;
    MWriter.Open( "ActualBeats.m", TRUE );
#endif

    ////////
    // Create streams with same info as input stream
    hr = pStrmOut->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmTempo->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmBeatPeriod->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    hr = pStrmBeatInfo->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;

    // Setup Param specifying onset input sampling rate
    ASSERT( g_BDParams.nOnsetSamplingRate == pStrmIn->GetSampleRate() );


#if FIND_BEATS
    ////////
    // Components Setup
    sIOIStats   IOIStats;

    CBDIOIStatCollector IOICollector;
    hr = IOICollector.Initialize( &IOIStats );
    if( FAILED(hr) )
        return hr;


    CBDNodeControl NodeControl;
    hr = NodeControl.Initialize();
    if( FAILED(hr) )
        return hr;


    ////////
    // Execute

    // Create sample "buffer" - points to actual onset stream data but could theoretically be
    // a finite length buffer (10 ms maybe) used by the fuzzy onset creation for look ahead
    FLOAT * pflSampleBuffer = ((FLOAT *)pStrmIn->GetData());
    // Current input sample #
    INT32 iCurSam = 0;
    CBDNode * pNodeBest = NULL;

    while( iCurSam < pStrmIn->GetNumSamples() )
    {
        /////////////////
        // Realtime Step

        // Track Performance
        if( !g_BDParams.fTrackPerformance && ((FLOAT)iCurSam/g_BDParams.nOnsetSamplingRate > g_BDParams.flTrackBeginOffset) )
            g_BDParams.fTrackPerformance = TRUE;

        
        // IOI Stats Collector - pass in only current sample
        hr = IOICollector.ExecuteStep( pflSampleBuffer[0], &IOIStats );

        
        // Node Control
        hr = NodeControl.ExecuteStep( pflSampleBuffer, &IOIStats );
        // Find best node
        pNodeBest = NodeControl.BestNode();

        /////////////////////
        // Update and Output
        if( NULL != pNodeBest )
        {
            // Output Samples
            ((FLOAT *)pStrmOut->GetData())[iCurSam] = pNodeBest->BeatOutput();
            ((FLOAT *)pStrmTempo->GetData())[iCurSam] = (pNodeBest->VarSampler()->SamplePeriod() - g_BDParams.flVarSamplerStartPeriod)*1000;//(pNodeBest->VarSampler()->SamplePeriod()-g_BDParams.flVarSamplerStartPeriod)*1000;//
            ((FLOAT *)pStrmBeatPeriod->GetData())[iCurSam] = pNodeBest->Period();//pNodeBest->VarSampler()->flE*1000;//pNodeBest->VarSampler()->m_flOffset*100;//
            ((FLOAT *)pStrmBeatInfo->GetData())[iCurSam] = pNodeBest->LoopComplete()*10;//pNodeBest->VarSampler()->fldE*1000;//(pNodeBest->VarSampler()->IdealSamplePeriod()- g_BDParams.flVarSamplerStartPeriod)*1000;//pNodeBest->CSNOutput();//(pNodeBest->VarSampler()->IdealSamplePeriod() - pNodeBest->VarSampler()->SamplePeriod())*1000;//
        }
        else
        {
            ((FLOAT *)pStrmOut->GetData())[iCurSam] = -1;
            ((FLOAT *)pStrmTempo->GetData())[iCurSam] = 0;
            ((FLOAT *)pStrmBeatPeriod->GetData())[iCurSam] = 0;
            ((FLOAT *)pStrmBeatInfo->GetData())[iCurSam] = 0;
        }
        
        /////////////////////


        #if 0
        if( iCurSam % 500 == 0 )
        {
            CHAR szLabel[50];
            sprintf( szLabel, "IOIHist_%d", iCurSam );
            MWriter.WriteFloatArray( szLabel, IOIStats.aflIOIHist, 800 );
        }
        #endif
        

        pflSampleBuffer++;
        iCurSam++;
    }

    ///////////////////////////////////////////////////
    // Calculate Performance Measures
    if( (NULL != pNodeBest) )
    {
        CBDNode * pNodeLongest = pNodeBest;
        for( NodeList::iterator iNode = NodeControl.m_lstNodes.begin(); 
             iNode != NodeControl.m_lstNodes.end(); 
             iNode++ )
        {
            if( (*iNode)->m_flSelectedTime > pNodeLongest->m_flSelectedTime )
            {
                pNodeLongest = *iNode;
            }
        }
        pNodeLongest->CalculatePerformanceMeasures();

        if( !((CBeatDetectApp *)AfxGetApp())->m_fAutomate )
        {
            FLOAT flBMP = 60/pNodeLongest->m_flAvgPeriod;
            FLOAT flPercentTime = pNodeLongest->m_flSelectedTime / (pStrmIn->GetDuration() - g_BDParams.flTrackBeginOffset);
            CString strMsg;
            strMsg.Format( "%% Time = %.2f\n%.2f BPM\n%.2f Error\n%d Beat Re-eval\n%d Node Changes", flPercentTime*100, flBMP, sqrt(pNodeLongest->m_flPredictionError), pNodeLongest->m_nBeatReEvaluations, g_BDParams.nTrackChangeNode );
            AfxMessageBox( strMsg );
        }
    }

    ///////////////////////////////////////////////////

    CMFileWriter<TimingLoopList> MWriter2;
    MWriter2.Open( "Beats.m", TRUE );
    //MWriter2.WriteFloatArray( "SamplePeriod", (FLOAT *)pStrmTempo->GetData(), pStrmTempo->GetNumSamples() );
    //MWriter2.WriteFloatArray( "MaxLoopPeriod", (FLOAT *)pStrmBeatPeriod->GetData(), pStrmBeatPeriod->GetNumSamples() );
    MWriter2.WriteFloatArray( "BeatOutput", (FLOAT *)pStrmOut->GetData(), pStrmOut->GetNumSamples() );
    //MWriter2.WriteFloatArray( "BeatInfo", (FLOAT *)pStrmBeatInfo->GetData(), pStrmBeatInfo->GetNumSamples() );
    //MWriter2.WriteTimingLoops( NodeControl.m_lstNodes );
    MWriter2.Close();
#endif


#if OUTPUT_ONSETS
    //MWriter.WriteFloatArray( "IOIHist_End", IOIStats.aflIOIHist, 800 );
    //MWriter.WriteFloatList( "IOIDom", IOIStats.lstDominantIOI );
    MWriter.WriteFloatArray( "Onsets", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );
    MWriter.Close();
#elif OUTPUT_ACTUALBEATS
    MWriter.WriteFloatArray( "ActualBeats", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );
    MWriter.Close();
#endif
   
    //WriteFloatArrayToMFile("OnsetStream.m", "OnsetStream", (FLOAT *)pStrmIn->GetData(), pStrmIn->GetNumSamples() );



    return hr;
}