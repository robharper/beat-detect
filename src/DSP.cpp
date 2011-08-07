// DSP.cpp: implementation of the CDSP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "BeatDetect.h"
#include "DSP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ****************************************************************
// NOTE: THIS CODE ASSUMES THE DATA IS NORMALIZED - FLOATING POINT
// ****************************************************************


static FILTER_6TH_COEFF LPF30Hz = 
{
    { 1,    -2.98860194684,    2.977268758941,  -0.9886666280522, 0, 0, 0 },
    { 2.300606208316e-008,6.901818624949e-008,6.901818624949e-008,2.300606208316e-008, 0, 0, 0 },
    0
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSP::CDSP()
{

}

CDSP::~CDSP()
{

}


//////////////////////////////////////////////////////////////////////
// Direct Form II - Up to 6th Order Filter
//
#pragma optimize( "atp", on )

HRESULT CDSP::DF2_Filter6
( 
    CDataStream* pStrmIn, 
    CDataStream* pStrmOut, 
    FILTER_6TH_COEFF* pC,
    BOOL fReverseGroupDelay
)
{
    HRESULT hr = S_OK;

    if( !pStrmIn->IsNormalized() )
        return E_INVALIDARG;

    // Delay units - allow up to 6-order filters
    
    DOUBLE dDelay[6] = {0, 0, 0, 0, 0, 0};
    DOUBLE dMiddle = 0;
    DOUBLE dX, dY;

    //                               dMiddle
    //  X ----> Sum ------ 1/a(0) ----->|--------- b(0) -----> Sum ----> Y
    //           ^                      |                       ^
    //           |                      v                       |     
    //          Sum <----- -a(1) ----- D(0) ------ b(1) -----> Sum
    //           ^                      |                       ^
    //           |                      v                       |     
    //          Sum <----- -a(2) ----- D(1) ------ b(2) -----> Sum
    //           .                      .                       .
    //           .                      .                       .
    //           .                      .                       .

    // Make the outstream the same params as the instream
    hr = pStrmOut->CreateData( pStrmIn );
    if( FAILED(hr) )
        return hr;

    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();
    
    DOUBLE * dA = pC->dA;
    DOUBLE * dB = pC->dB;

    INT32 nInSamples = pStrmIn->GetNumSamples();
    INT32 nInSamplesPlusDelay = nInSamples + pC->nSamplesDelay;

    if( !fReverseGroupDelay )
        return E_FAIL;

    BOOL  fDone = FALSE;
    INT32 iSam = 0;
    while( iSam < nInSamplesPlusDelay )
    {
        // Input value from input stream or zero if past end of input stream
        dX = 0;
        if( iSam < nInSamples )
            dX = *pflDataIn++;

        // Calculate
        dMiddle = (dX - dDelay[0]*dA[1] - dDelay[1]*dA[2] - dDelay[2]*dA[3]
                      - dDelay[3]*dA[4] - dDelay[4]*dA[5] - dDelay[5]*dA[6])/dA[0];

        dY = dMiddle*dB[0] + dDelay[0]*dB[1] + dDelay[1]*dB[2] + dDelay[2]*dB[3]
                           + dDelay[3]*dB[4] + dDelay[4]*dB[5] + dDelay[5]*dB[6];

        // Set output value to stream if no reverse group delay, or only start setting to
        // stream after group delay has passed
        if( iSam >= pC->nSamplesDelay )
            *pflDataOut++ = (FLOAT)dY;
        
        // Update Delays
        dDelay[5] = dDelay[4];
        dDelay[4] = dDelay[3];
        dDelay[3] = dDelay[2];
        dDelay[2] = dDelay[1];
        dDelay[1] = dDelay[0];
        dDelay[0] = dMiddle;

        iSam++;

        // Done if through all samples and no reverse group delay, or if through all samples
        // plus group delay samples
    }

    return hr;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Convolution by a variable length kernel
#pragma optimize( "atp", on )

HRESULT CDSP::Convolve
(
    CDataStream *pStrmIn, 
    CDataStream *pStrmOut, 
    const FLOAT aflKernel[], 
    const INT32 nKernelLen
)
{
    HRESULT hr = S_OK;

    if( !pStrmIn->IsNormalized() )
        return E_INVALIDARG;

    // Create output stream same as input but longer by kernel length - 1
    pStrmOut->CreateData( pStrmIn->GetBitsPerSample(), 
                          pStrmIn->GetSampleRate(),
                          pStrmIn->GetNumSamples() + nKernelLen - 1,
                          pStrmIn->IsNormalized() );

    //
    // NOTE: THIS CODE ASSUMES THE DATA IS 16-BITS PER SAMPLE!!
    //
    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();

    for( INT32 iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        FLOAT flOutput = 0;
        INT32 iLeftLimit = max(iSam-nKernelLen+1, 0);
        INT32 iRightLimit = min(iSam+1, pStrmIn->GetNumSamples());
        for( INT32 iK=iLeftLimit; iK < iRightLimit; iK++ )
        {
            flOutput += (FLOAT)(pflDataIn[iK] * aflKernel[iSam-iK]);
        }
        pflDataOut[iSam] = flOutput;
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////
// Root-Mean-Square Decimate signal
#pragma optimize( "atp", on )

HRESULT CDSP::RMSDecimate
(   
    CDataStream *pStrmIn, 
    CDataStream *pStrmOut, 
    INT32 nDec
)
{
    HRESULT hr = S_OK;

    if( !pStrmIn->IsNormalized() )
        return E_INVALIDARG;

    // Create working stream
    CDataStream strmTemp;

    // Create output stream as decimated input
    pStrmOut->CreateData( pStrmIn->GetBitsPerSample(), 
                          pStrmIn->GetSampleRate() / nDec,
                          (INT32)ceil((DOUBLE)pStrmIn->GetNumSamples() / nDec),
                          pStrmIn->IsNormalized() );

    FLOAT * pflDataIn = (FLOAT *)pStrmIn->GetData();
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();

    ///////////////////////////////////////////////////////////
    // Sqaure the input signal
    for( INT32 iSam=0; iSam<pStrmIn->GetNumSamples(); iSam++ )
    {
        pflDataIn[iSam] = pflDataIn[iSam]*pflDataIn[iSam];
    }

    // LPF input signal
    hr = DF2_Filter6( pStrmIn, &strmTemp, &LPF30Hz, TRUE );
    if( FAILED(hr) )
        return hr;

    FLOAT * pflDataTemp = (FLOAT *)strmTemp.GetData();

    // Root and Decimate the signal
    for( iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        FLOAT flResult = 0;
        INT32 iRightLimit = min((iSam+1)*nDec, strmTemp.GetNumSamples());
        
        for( INT32 ii=iSam*nDec; ii < iRightLimit; ii++ )
        {
            flResult += (FLOAT)sqrt( max(pflDataTemp[ii],0) );
        }
        
        pflDataOut[iSam] = flResult/nDec;
    }
    ///////////////////////////////////////////////////////////

    /*
    for( INT32 iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        FLOAT flResult = 0;
        INT32 iRightLimit = min((iSam+1)*nDec, pStrmIn->GetNumSamples());
        
        for( INT32 ii=iSam*nDec; ii < iRightLimit; ii++ )
        {
            flResult += pflDataIn[ii]*pflDataIn[ii];
        }
        
        pflDataOut[iSam] = (FLOAT)sqrt(flResult)/nDec;
    }
    */

    return hr;
}


#pragma optimize( "atp", on )

HRESULT CDSP::Mix
(
    CDataStream *pStrmIn1, 
    FLOAT flVol1, 
    CDataStream *pStrmIn2, 
    FLOAT flVol2, 
    CDataStream *pStrmOut
)
{
    if( !pStrmIn1->IsNormalized() || !pStrmIn2->IsNormalized() )
        return E_INVALIDARG;
    if( pStrmIn1->GetSampleRate() != pStrmIn2->GetSampleRate() )
        return E_INVALIDARG;

    INT32 nSamples = min( pStrmIn1->GetNumSamples(), pStrmIn2->GetNumSamples() );

    HRESULT hr;
    hr = pStrmOut->CreateData( pStrmIn1->GetBitsPerSample(), pStrmIn1->GetSampleRate(), nSamples, TRUE );
    if( FAILED(hr) )
        return hr;

    FLOAT * pflDataIn1 = (FLOAT *)pStrmIn1->GetData();
    FLOAT * pflDataIn2 = (FLOAT *)pStrmIn2->GetData();
    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();

    for( INT32 iSam = 0; iSam < nSamples; iSam++ )
    {
        pflDataOut[iSam] = pflDataIn1[iSam]*flVol1 + pflDataIn2[iSam]*flVol2;
    }

    return hr;
}
