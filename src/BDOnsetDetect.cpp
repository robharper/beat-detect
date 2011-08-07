// BDOnsetDetect.cpp: implementation of the CBDOnsetDetect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "DSP.h"
#include "BDOnsetDetect.h"
#include "BDUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Half-Hanning (Raised Cosine) window
// 100ms duration at 441Hz sampling rate
static const FLOAT aflHalfHanning100ms[] = 
{ 0.9997f,    0.9972f,    0.9922f,    0.9848f,    0.9750f,    0.9628f,    0.9483f,    0.9315f,    0.9126f,
  0.8917f,    0.8688f,    0.8441f,    0.8176f,    0.7896f,    0.7601f,    0.7294f,    0.6974f,    0.6645f,
  0.6308f,    0.5965f,    0.5616f,    0.5265f,    0.4912f,    0.4559f,    0.4209f,    0.3863f,    0.3522f,
  0.3189f,    0.2865f,    0.2551f,    0.2250f,    0.1962f,    0.1689f,    0.1433f,    0.1195f,    0.0976f,
  0.0776f,    0.0598f,    0.0442f,    0.0308f,    0.0198f,    0.0112f,    0.0050f,    0.0012f };
static const INT32 nHalfHanning100ms = 44;


// Number of steps of slope to calculate and normalization factor
// Normalization is Sum(1/i), i=1 to Steps
#define ENV_NORMALIZE   2.9290f
#define ENV_STEPS       10


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
// ***** Thresholds must be set with some degree of intelligence *****
//
CBDOnsetDetect::CBDOnsetDetect()
{

}

CBDOnsetDetect::~CBDOnsetDetect()
{

}


HRESULT CBDOnsetDetect::CreateOnsetStream
(
    CAudioStream *pStrmIn, 
    CDataStream *pStrmOut,
    CDataStream *pStrmInternal
)
{
    HRESULT hr = S_OK;

    pStrmInternal->ReleaseData();

    CDataStream StrmDecimated, StrmProcessed;

    //////
    hr = CDSP::RMSDecimate( pStrmIn, &StrmDecimated, pStrmIn->GetSampleRate()/g_BDParams.nOnsetSamplingRate );
    if( FAILED(hr) )
        return hr;

    //////
    hr = CDSP::Convolve( &StrmDecimated, &StrmProcessed, aflHalfHanning100ms, nHalfHanning100ms );
    if( FAILED(hr) )
        return hr;
   
    //////
    hr = ProcessEnvelope( &StrmProcessed, pStrmInternal );
    if( FAILED(hr) )
        return hr;

    //////
    hr = ThresholdStream( pStrmInternal, &StrmProcessed, pStrmOut );
    if( FAILED(hr) )
        return hr;

    return hr;
}


HRESULT CBDOnsetDetect::ProcessEnvelope
(
    CDataStream *pStrmIn, 
    CDataStream *pStrmOut
)
{
    HRESULT hr = S_OK;

    //////
    // Calculate onset detection threshold function
    hr = pStrmOut->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();
    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    for( INT32 iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        // Seppanen/Klapuri
 /*
        if( pflDataIn[iSam]+pflDataIn[iSam-1] > 0 )
            pflDataOut[iSam] = (pflDataIn[iSam]-pflDataIn[iSam-1]) / (pflDataIn[iSam]+pflDataIn[iSam-1]);
        else
            pflDataOut[iSam] = 0;
 */

        // Scheirer - LOUSY
        //psDataOut[iSam] = 50*(psDataIn[iSam]-psDataIn[iSam-1]);
        
        // Duxbury
        /*
        DOUBLE dResult = psDataIn[iSam];
        INT32 iLeftLimit = max(iSam-30,0);
        for( INT32 ii=iLeftLimit; ii<iSam; ii++ )
        {
            dResult -= (DOUBLE)psDataIn[ii]/(iSam-ii);
        }
        psDataOut[iSam] = (signed short)(10*dResult);
        */

        // Duxbury/Klapuri
        FLOAT flResult = 0;
        INT32 iLeftLimit = max(iSam-ENV_STEPS,0);
        for( INT32 ii=iLeftLimit; ii<iSam; ii++ )
        {
            FLOAT flTemp = (pflDataIn[iSam]-pflDataIn[ii]);
            if( flTemp != 0 )   // Don't want to divide by zero!
                flResult += flTemp/((iSam-ii)*(pflDataIn[iSam]+pflDataIn[ii]));
        }
        pflDataOut[iSam] = flResult / ENV_NORMALIZE;
     

        // Second Difference - LOUSY
        //psDataOut[iSam] = 50*((psDataIn[iSam]-psDataIn[iSam-1])-(psDataIn[iSam-1]-psDataIn[iSam-2]));
    }

    return hr;
}


HRESULT CBDOnsetDetect::ThresholdStream
(
    CDataStream *pStrmIn, 
    CDataStream *pStrmEnv,
    CDataStream *pStrmOut
)
{
    HRESULT hr = S_OK;

    enum { ThreshFound, ThreshLooking } eState = ThreshLooking;

    //////
    // Calculate onset detection threshold function
    hr = pStrmOut->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;
    
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();
    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    FLOAT * pflEnv = (FLOAT *)pStrmEnv->GetData();

    // Calculate minimum distance to pass before another sample may be detected
    INT32 nSamMinDist = (INT32)(g_BDParams.flOnsetDetectResetTime * pStrmIn->GetSampleRate());

    INT32 iLastFound = 0;
    for( INT32 iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        if( (eState == ThreshLooking) && (pflDataIn[iSam] > g_BDParams.flOnsetDetectThreshHigh) )
        {
            //////
            // Found onset, update state
            eState = ThreshFound;
            iLastFound = iSam;

            // Determine the intensity of this onset - search for maximum level of envelope
            // Intensity = env(max) - env(begin)
            FLOAT flEnvMax = 0;

            while( (pflDataIn[iSam] > 0) && (iSam-iLastFound < nSamMinDist) )
            { 
                if( pflEnv[iSam] > flEnvMax )
                    flEnvMax = pflEnv[iSam]; 
                iSam++;
                pflDataOut[iSam] = 0;       // Zero out these subsequent searched samples' onset sample
            }

            pflDataOut[iLastFound] = flEnvMax - pflEnv[iLastFound];
        }
        else if( (eState == ThreshFound) && ((pflDataIn[iSam] < g_BDParams.flOnsetDetectThreshLow) || 
                 (iSam > iLastFound+nSamMinDist )) )
        {
            //////
            eState = ThreshLooking;
            pflDataOut[iSam] = 0;
        }
        else
        {
            //////
            pflDataOut[iSam] = 0;
        }
    }

    return hr;
}
