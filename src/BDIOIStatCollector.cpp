// BDIOIStatCollector.cpp: implementation of the CBDIOIStatCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDIOIStatCollector.h"
#include "BDUtils.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDIOIStatCollector::CBDIOIStatCollector() : m_nMaxIOI(0)
{

}

CBDIOIStatCollector::~CBDIOIStatCollector()
{

}


HRESULT     CBDIOIStatCollector::Initialize
(
    sIOIStats * pStats
)
{
    HRESULT hr = S_OK;

    // Reset onset list
    m_lstOnset.clear();

    // Clear IOIStats Histogram and Dominant IOI List
    memset( pStats->aflIOIHist, 0, sizeof(pStats->aflIOIHist) );
    pStats->lstDominantIOI.clear();

    m_nLastOnsetDelay = 0;

    // Create Parzen Window
    FLOAT flVar = (FLOAT)(IOISTATS_PARZEN_HALF_WINDOW_SIZE*IOISTATS_PARZEN_HALF_WINDOW_SIZE)/5;
    for( INT32 ii=0; ii<IOISTATS_PARZEN_WINDOW_SIZE; ii++ )
    {
        FLOAT flx = ii-IOISTATS_PARZEN_HALF_WINDOW_SIZE;
        
        m_aflParzenWindow[ii] = (FLOAT) exp( - flx*flx / flVar );
    }

    return hr;
}


HRESULT     CBDIOIStatCollector::ExecuteStep
( 
    FLOAT flSample,
    sIOIStats * pStats 
)
{
    HRESULT hr = S_OK;

    m_nLastOnsetDelay += 1;

    if( flSample > 0 )
    {
        /////
        // We have received an onset!

        m_nMaxIOI = g_BDParams.flIOIMaxOnsetTime * g_BDParams.nOnsetSamplingRate;
        ASSERT( m_nMaxIOI < IOISTATS_HISTLEN );

        // Increment onset times by newest IOI
        for( OnsetList::iterator iter = m_lstOnset.begin(); iter != m_lstOnset.end(); ++iter )
        {
            (*iter) += m_nLastOnsetDelay;
        }

        // Add newest IOI to queue
        m_lstOnset.push_front( m_nLastOnsetDelay );

        // Look through list for IOIs that are greater than MaxIOI and remove
        if( m_lstOnset.size() > 0 )
        { 
            while( m_lstOnset.back() > m_nMaxIOI )
            {
                m_lstOnset.pop_back();
            }
        }

        /////
        // Update the IOIStats Histogram
        // Decay Histogram - by amount proportional to passed time
        // m_nLastOnsetDelay*flPeriod equals time passed since last onset
        FLOAT flDecay = pow(0.5, (m_nLastOnsetDelay/g_BDParams.nOnsetSamplingRate)/g_BDParams.flIOIHistHalflife );
        for( INT32 ii=0; ii < IOISTATS_HISTLEN; ii++ )
        {
            pStats->aflIOIHist[ii] *= flDecay;
        }
        

        // Grow Histogram - inversely proportional to histogram half-life
        // 0.693 = ln2 : FROM SEPPANNEN
        FLOAT flGrow = 0.693/g_BDParams.flIOIHistHalflife;     
        for( iter = m_lstOnset.begin(); iter != m_lstOnset.end(); ++iter )
        {
            // Use parzen window growth
            for( INT32 iParzen = max(0,(*iter)-IOISTATS_PARZEN_HALF_WINDOW_SIZE); 
                 iParzen < (*iter) + IOISTATS_PARZEN_HALF_WINDOW_SIZE;
                 iParzen++ )
            {
                pStats->aflIOIHist[iParzen] += m_aflParzenWindow[iParzen - (*iter-IOISTATS_PARZEN_HALF_WINDOW_SIZE)];
            }
        }


        //////
        // Find Peak IOIs
        hr = FindDominantIOIs( (FLOAT)1/g_BDParams.nOnsetSamplingRate, pStats );

        
        // Reset last delay...
        m_nLastOnsetDelay = 0;
    }

    return hr;
}


HRESULT     CBDIOIStatCollector::FindDominantIOIs
( 
    FLOAT flPeriod,
    sIOIStats * pStats 
)
{
    // Empty the list and regenerate
    pStats->lstDominantIOI.clear();

    // Collect Stats (Mean, Max)
    FLOAT flMean = 0, flMax = 0;
    for( INT32 ii=0; ii < m_nMaxIOI; ii++ )
    {
        flMean += pStats->aflIOIHist[ii];
        if( pStats->aflIOIHist[ii] > flMax )
            flMax = pStats->aflIOIHist[ii];
    }
    flMean /= m_nMaxIOI;

    // Threshold as ratio between max and mean
    FLOAT flThresh = flMax * g_BDParams.flIOIDomThreshRatio + flMean * (1-g_BDParams.flIOIDomThreshRatio);

    // Add IOIs to list that are over threshold
    // Start search at minimum allowed period since should never be that low and simplifies code
    INT32 nStartIOI = 0.24f * g_BDParams.nOnsetSamplingRate;
    for( ii=nStartIOI; ii < m_nMaxIOI; ii++ )
    {
        if( pStats->aflIOIHist[ii] > flThresh )
        {
            // Sufficient intensity, look for peak
            BOOL fPeak =   (pStats->aflIOIHist[ii]>pStats->aflIOIHist[ii-1]) &&
                           (pStats->aflIOIHist[ii]>pStats->aflIOIHist[ii-2]) &&
                           (pStats->aflIOIHist[ii]>pStats->aflIOIHist[ii+1]) &&
                           (pStats->aflIOIHist[ii]>pStats->aflIOIHist[ii+2]);

            // Add this dominant IOI to list
            if( fPeak )
            {
                // Use Newton Interpolation (pg 104 of notes)
                // Three point interpolation, finding maximum (peak)
                sIOIPeriodEntry sEntry;
                sEntry.flPeriod = ((FLOAT)ii-0.5f) - ((FLOAT)(pStats->aflIOIHist[ii]-pStats->aflIOIHist[ii-1])/
                                                      (pStats->aflIOIHist[ii+1]-2*pStats->aflIOIHist[ii]+pStats->aflIOIHist[ii-1]));
                sEntry.flPeriod *= flPeriod;
                sEntry.nRefCount = 0;
                sEntry.flHighestEnergy = 0;
                pStats->lstDominantIOI.push_back( sEntry );
            }
        }
    }

    return S_OK;
}












