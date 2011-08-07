// BDNodeVarSampler.cpp: implementation of the CBDNodeVarSampler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDNodeVarSampler.h"
#include "BDNode.h"
#include "BDNodeTimingNet.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ERROR_HIST_LENGTH       5
#define ERROR_HIST_ALPHA        0.8f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDNodeVarSampler::CBDNodeVarSampler( CBDNode * pNode ) : m_pNode(pNode)
{

}

CBDNodeVarSampler::~CBDNodeVarSampler()
{

}


HRESULT CBDNodeVarSampler::Initialize
( 
    FLOAT flSamplerPeriod 
)
{
    HRESULT hr = S_OK;

    // Sampler Init
    m_flPeriod = flSamplerPeriod;
    m_flIdealPeriod = flSamplerPeriod;

    m_flBeginSam = 0;
    m_flEndSam = m_flPeriod * g_BDParams.nOnsetSamplingRate;
    m_iCurSam = 0;

    m_flEnergyRemainder = 0;
    m_iLastOnset = INT_MIN;
    m_flLastOnsetEnergy = 0;
    m_flLastBeatTime = 0;

    m_flIdealPeriodWeight = 0;
    m_flLastExpWeight = 1;


    // Variable Sampler Init
    m_fFoundBeat = FALSE;
    m_flOffset = 0;

    return hr;
}


HRESULT CBDNodeVarSampler::ProcessInput
( 
    FLOAT * aflInputBuffer,
    BOOL *  pfSampleComplete, 
    FLOAT * pflSample 
)
{
    HRESULT hr = S_OK;

    // Onset?
    if( aflInputBuffer[0] > 0 )
    {
        m_iLastOnset = m_iCurSam;
        m_flLastOnsetEnergy = aflInputBuffer[0];
    }

    // Increment current sample
    m_iCurSam += 1;

    // Complete this sample period?
    if( m_iCurSam > m_flEndSam )
    {
        *pflSample = 0;

        /////////////////////////////
        // Calculate fuzzy onset energy

        FLOAT   flFuzzyWidth = g_BDParams.flFuzzyOnsetWidth * g_BDParams.nOnsetSamplingRate / 2;
        
        if( m_iLastOnset > m_flBeginSam - flFuzzyWidth )
        {
            // Which case is it?  Or none = no onset!
            //              |___________|
            //  |..W.....W..|..W.....W..|..W...
            //             A B    C    D E
            // Case A: Previous sampling interval within fuzzy width (W) of edge
            if( m_iLastOnset < m_flBeginSam )
            { 
                // Remaining energy variable should be accurate, use it...
                *pflSample = m_flEnergyRemainder;
            }
            // Case B: This interval, but within fuzzy width (W) of the beginning
            else if( m_iLastOnset < m_flBeginSam + flFuzzyWidth ) 
            {
                // Remaining energy variable not necessarily valid, calculate in full
                FLOAT flDist = m_flBeginSam - (m_iLastOnset - flFuzzyWidth);
                *pflSample = m_flLastOnsetEnergy - (m_flLastOnsetEnergy / (2*flFuzzyWidth*flFuzzyWidth)) * flDist * flDist;
            }
            // Case C: This interval and not near an edge, so no fuzziness
            else if( m_iLastOnset < m_flEndSam - flFuzzyWidth ) 
            {
                // No fuzzy energy sharing
                *pflSample = m_flLastOnsetEnergy;
            }
            // Case D: Withing fuzzy width of end of sampling interval
            else 
            {
                // Calculate energy and place remainder in remaining energy variable
                FLOAT flDist = (m_iLastOnset + flFuzzyWidth) - m_flEndSam;
                m_flEnergyRemainder = (m_flLastOnsetEnergy / (2*flFuzzyWidth*flFuzzyWidth)) * flDist * flDist;
                *pflSample = m_flLastOnsetEnergy - m_flEnergyRemainder;
            }
        }
        else
        {
            // Check Case E: Search beginning of next sampling interval in buffer for onset
            for( INT32 iSearchSam=1; iSearchSam < flFuzzyWidth; iSearchSam++ )
            {
                if( aflInputBuffer[iSearchSam] > 0 )
                {
                    // Calculate energy, don't bother with remaining energy since we'll always recalculate
                    FLOAT flDist = (m_iCurSam-1) + iSearchSam - m_flEndSam;
                    *pflSample = (aflInputBuffer[iSearchSam] / (2*flFuzzyWidth*flFuzzyWidth)) * flDist * flDist;
                    break;
                }
            }
        }
        
        /////////////////////////////
        // Variable Sampler Execution
        HRESULT hrTest = AdjustSamplingRate();
        if( hrTest == S_OK )
            m_pNode->AdjustPeriod();

        /////////////////////////////
        // Update variables, continue
        m_flBeginSam = m_flEndSam;
        m_flEndSam += m_flPeriod * g_BDParams.nOnsetSamplingRate;
        *pfSampleComplete = TRUE;
    }
    else
    {
        // Not yet complete
        *pfSampleComplete = FALSE;
    }

    return hr;
}


HRESULT CBDNodeVarSampler::AdjustSamplingRate
( 
)
{
    HRESULT hr = S_FALSE;

    // Get the current beat output (remember, we have not yet sent this sample out the door
    //   so the beat output given here corresponds to the current range of samples)
    FLOAT flBeat = m_pNode->TimingNet()->BeatOutputPrediction();

    // If a beat occurred, set it's location to be dead centre of sampling interval
    if( flBeat > 0 )
    {
        // Record recent beat location and recent onset location
        m_fFoundBeat = TRUE;
        m_flRecentBeat = m_flBeginSam + m_flPeriod/2;
        m_flRecentBeatEnergy = flBeat;
        m_flRecentOnset = (FLOAT)m_iLastOnset;
        // But wait, what if a beat and onset come at the same time, well then we know that "last
        // onset" *MUST* be the closest onset by definition, so just go with it
    }

    if( m_fFoundBeat )
    {
        // We've found the beat, now we're looking after the beat for the possibility of finding
        // the nearest onset.  But ONLY search as far as the minimum of the distance to the
        // most recent onset (because after that we know the previous onset is closer) and the 
        // maximum search distance (can't search forever)

        BOOL fAdjust = FALSE;
        FLOAT flSearchDist = m_flEndSam - m_flRecentBeat;
        FLOAT flOffset = 0;

        
        if( m_flRecentOnset != (FLOAT)m_iLastOnset )
        {
            // Last onset is not the only onset we've found - we're set to adjust!
            fAdjust = TRUE;

            FLOAT flDist1 = m_flRecentOnset - m_flRecentBeat;
            FLOAT flDist2 = m_iLastOnset - m_flRecentBeat;
            
            flOffset = fabs(flDist1) < fabs(flDist2) ? flDist1 : flDist2;
        }
        else if( (flSearchDist > (m_flRecentOnset-m_flRecentBeat)) ||
                 (flSearchDist > (g_BDParams.flVarSamplerMaxErrorTime * g_BDParams.nOnsetSamplingRate)) )
        {
            // Searched too long, time to adjust
            fAdjust = TRUE;

            flOffset = m_flRecentOnset - m_flRecentBeat;
        }

        //////////////////
        // Time to adjust?
        if( fAdjust && fabs(flOffset/g_BDParams.nOnsetSamplingRate) < g_BDParams.flVarSamplerMaxErrorTime )
        {
            // Track Performance
            if( m_pNode->m_fSelected )
            {
                m_pNode->m_flPredictionError += flOffset*flOffset;
                m_pNode->m_nSelectedBeats++;
            }


            FLOAT flSqrBeatEnergy = sqrt(m_flRecentBeatEnergy);

            // Anything 0.5 samples or smaller is the smallest error possible here, so treat
            // it as no error by setting the offset to zero
            if( fabs(flOffset) <= 0.5 )
                flOffset = 0;

            
            // Calculate expectancy weight            
            // StdDev proportional to loop length
            FLOAT flStdDev = (m_pNode->Period()*g_BDParams.flExpectationDeviancePercent) * g_BDParams.nOnsetSamplingRate;
            flStdDev = min( flStdDev, g_BDParams.flExpectationStdDevSamples * m_flPeriod * g_BDParams.nOnsetSamplingRate );
            FLOAT flWeight = (FLOAT)exp( - ((flOffset*flOffset)/(flStdDev*flStdDev)) );
            

            // Convert offset sample distance into time - use sampling rate of input
            // signal since the # of sample error is at this sampling rate
            flOffset /= g_BDParams.nOnsetSamplingRate;

            
            // PD Controller Error Input
            FLOAT flLastOffset = m_flOffset;
            m_flOffset = flOffset;

            ///////////////////////////////////////////////////////////////////////
            // PD Controller
            // Error in seconds error per sample of the loop
            flE = m_flOffset / m_pNode->TimingNet()->LoopLength();
            // Differential Error in seconds error change per sample of the loop
            fldE = (m_flOffset - flLastOffset) / (m_flRecentBeat - m_flLastBeatTime);
            
            m_flIdealPeriod = (m_flPeriod + flWeight * g_BDParams.flVarSamplerGainProp * flE + 
                                            sqrt(flWeight*m_flLastExpWeight) * g_BDParams.flVarSamplerGainDiff * fldE);

            // Calculate expectation weighting for this period calculation
            //m_flIdealPeriodWeight = sqrt(flWeight*m_flLastExpWeight);
            m_flLastExpWeight = flWeight;

            hr = S_OK;
            //////////////////////////////////////////////////////////////////////
            
            // Remember time of this beat for next time
            m_flLastBeatTime = m_flRecentBeat;
            // We're done here so reset and start over
            m_fFoundBeat = FALSE;
        }
    }

    return hr;
}


