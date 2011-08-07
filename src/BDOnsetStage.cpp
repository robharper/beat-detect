// BDOnsetStage.cpp: implementation of the CBDOnsetStage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDOnsetStage.h"
#include "BDOnsetDetect.h"
#include "DSP.h"
#include "BDUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Filters
//////////////////////////////////////////////////////////////////////

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// LIMITING BANDS TO 4 - CHECK THROUGH THIS CODE IF YOU WANT TO CHANGE
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


FILTER_6TH_COEFF aIIR_BAND_FILTERS[NUM_BANDS] = 
{
// LP: 110
{
    {
                     1,    -5.939446833066,    14.69906478643,   -19.40175578925,
        14.40534742567,   -5.704452460387,     0.9412428706195
    },
    {
        2.246293429042e-013,    1.347776057425e-012,    3.369440143564e-012,    4.492586858085e-012,
        3.369440143564e-012,    1.347776057425e-012,    2.246293429042e-013
    },
    301
},
// BP: 110-220
{
    {
                     1,    -5.967189626739,    14.83793472029,   -19.67980659146,
        14.68370868926,   -5.843788343797,     0.9691411525664
    },
    {
        4.737344671204e-007,                 0,     -1.421203401361e-006,                 0,
        1.421203401361e-006,                 0,     -4.737344671204e-007
    },
    302
},
// BP: 220-440
{
    {
                     1,    -5.931480316493,    14.66543773155,   -19.34664873533,
        14.36213855651,   -5.688679994152,     0.9392327652624
    },
    {
        3.731600537578e-006,                 0,     -1.119480161273e-005,                 0,
        1.119480161273e-005,                 0,     -3.731600537578e-006
    },
    151
},
// BP: 440-880
{
    {
                     1,     -5.85156216959,    14.29071167394,   -18.64466322718,
          13.705592882,    -5.382223290893,   0.8821445873275
    },
    {
        2.895266535932e-005,                 0,     -8.685799607795e-005,                 0,
        8.685799607795e-005,                 0,     -2.895266535932e-005
    },
    75
},
// BP: 880-1760
{
    {
                     1,    -5.659143272881,    13.43390065645,   -17.12108832999,
        12.35540348684,   -4.787127980137,    0.7780827445559
    },
    {
        0.0002181740519493,                 0,  -0.0006545221558478,                 0,
        0.0006545221558478,                 0,  -0.0002181740519493
    },
    38
},
// BP: 1760-3520
{
    {
                     1,    -5.155728149138,    11.39021960408,   -13.78148945915,
        9.629218366135,   -3.685501984199,     0.6048042415924
    },
    {
        0.001557156728436,                 0,   -0.004671470185309,                 0,
        0.004671470185309,                 0,   -0.001557156728436
    },
    19
},
// BP: 3520-7040
{
    {
                     1,    -3.773089717521,    6.812932868527,   -7.272198696119,
        4.854306593905,   -1.912618474691,    0.3626510747757
    },
    {
        0.0101224047336,                 0,   -0.03036721420079,                 0,
        0.03036721420079,                0,   -0.0101224047336
    },
    9
},
// BP: 7040-14080
{
    {
                       1,  -0.3056501360981,    1.080961146038,  -0.2542770672661,
         0.6428131702076, -0.07427855753132,   0.1187222370285
    },
    {
        0.05757480091287,                 0,  -0.1727244027386,                 0,
        0.1727244027386,                  0,  -0.05757480091287
    },
    5
},
/*
// HP: 14080+
{
    {
                      1,   1.64639815544,      1.7936603173,       1.081750428817,
        0.4245502003134,  0.09188386110321,   0.008968391946871
    },
    {
        0.006361663503134, -0.03816998101881,  0.09542495254701,  -0.1272332700627,
        0.09542495254701,  -0.03816998101881,  0.006361663503134
    },
    4
}
*/
};




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDOnsetStage::CBDOnsetStage()
{

}

CBDOnsetStage::~CBDOnsetStage()
{

}


//////////////////////////////////////////////////////////////////////
// Processing
//////////////////////////////////////////////////////////////////////

HRESULT CBDOnsetStage::CreateOnsetStream
( 
    CAudioStream *pStrmIn, 
    CDataStream *pStrmOut,
    CDataStream *pStrmInternal
)
{
    if( !pStrmIn->IsValid() )
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    ////////////////////////////////////
    // BandPass Filter the input audio and detect onsets
    
    CBDOnsetDetect OnsetDetect;

    for( INT32 ii=NUM_BANDS-1; ii>=0; ii-- )
    {
        hr = CDSP::DF2_Filter6( pStrmIn, &m_aStrmBandInput[ii], &(aIIR_BAND_FILTERS[ii]), TRUE );
        if( FAILED(hr) )
            break;
        hr = OnsetDetect.CreateOnsetStream( &m_aStrmBandInput[ii], &m_aStrmOnset[ii], pStrmInternal );
        if( FAILED(hr) )
            break;
    }
    if( FAILED(hr) )
        return hr;
    
    // Reassemble onset streams into one conglomerate stream
    hr = ReassembleOnsets( pStrmOut );
    if( FAILED(hr) )
        return hr;
    
    return hr;
}


// Split the input signal into its separate frequency bands
HRESULT CBDOnsetStage::BandSplitInput
( 
    CAudioStream *pStrmIn 
)
{
    return S_OK;
}


// Reassemble onset streams into one output stream
HRESULT CBDOnsetStage::ReassembleOnsets
( 
    CDataStream *pStrmOut 
)
{
    HRESULT hr = S_OK;

    // Stream out is same format as all onset streams
    pStrmOut->CreateData( &m_aStrmOnset[0] );

    FLOAT * pflDataOut = (FLOAT *)pStrmOut->GetData();

    // Band Weights - Weight higher bands more (Duxbury)
    //FLOAT aflBandWeight[NUM_BANDS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    FLOAT aflBandWeight[NUM_BANDS] = { 1, 1, 1, 1, 1, 1, 1, 1, };

    //////
    // Create Union of all Onsets
    for( INT32 iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        // Union onsets from all bands
        FLOAT flTotal = 0;
        for( INT32 ii=0; ii<NUM_BANDS; ii++ )
            flTotal += ((FLOAT *)m_aStrmOnset[ii].GetData())[iSam] * aflBandWeight[ii];

        pflDataOut[iSam] = flTotal;
    }

    INT32 nMinOnsetDist = (INT32)(g_BDParams.flOnsetCombineMinDist * pStrmOut->GetSampleRate());

    //////
    // Filter union of onsets to remove weaker/too close onsets
    // Option 1: Simple - onset found, use strongest onset in next nMinOnsetDist samples
    /*
    for( iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        // Found an onset?
        if( pflDataOut[iSam] > 0 )
        {
            // **** HURTING ****
            FLOAT flMaxOnset = 0;
            INT32 iSamMax = 0;

            // Search through the next m_nSamMinOnsetDist samples
            INT32 iStart = iSam;
            for( ; iSam < iStart + nMinOnsetDist; iSam++ )
            {
                if( pflDataOut[iSam] > flMaxOnset )
                {
                    iSamMax = iSam;
                    flMaxOnset = pflDataOut[iSam];
                }
                // Clear this range of all onsets
                pflDataOut[iSam] = 0;
            }
            // Set one offset at best location found
            pflDataOut[iSamMax] = flMaxOnset;
        }
    }
    */

    ////////
    // Filter union of onsets to remove weaker/too close onsets
    // Option 3: Sparse - always start search from strongest onset nMinOnsetDist forward
    // restarting search if a stronger onset is found
    for( iSam=0; iSam<pStrmOut->GetNumSamples(); iSam++ )
    {
        // Found an onset?
        if( pflDataOut[iSam] > 0 )
        {
            FLOAT flMaxOnset = pflDataOut[iSam];
            INT32 iSamMax = iSam;

            // Search through the next m_nSamMinOnsetDist samples
            INT32 iStart = iSam;
            for( ; (iSam < iStart + nMinOnsetDist) && (iSam < pStrmOut->GetNumSamples()); iSam++ )
            {
                if( pflDataOut[iSam] > flMaxOnset )
                {
                    iSamMax = iSam;     // Found stronger, remember its location
                    iStart = iSam;      // Restart search here (on now largest onset)
                    flMaxOnset = pflDataOut[iSam];
                }
                // Clear this range of all onsets
                pflDataOut[iSam] = 0;
            }
            // Set one offset at best location found
/*      
            if( flMaxOnset >= g_BDParams.flOnsetCombineMinOnset )
                pflDataOut[iSamMax] = 1;//flMaxOnset;
            else
                pflDataOut[iSamMax] = 0.5f;
*/           
            pflDataOut[iSamMax] = flMaxOnset;
        }
    }
    return hr;
}