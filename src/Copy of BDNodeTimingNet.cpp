// BDNodeTimingNet.cpp: implementation of the CBDNodeTimingNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDNodeTimingNet.h"
#include "BDNode.h"
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

CBDNodeTimingNet::CBDNodeTimingNet( CBDNode * pNode ) : m_pNode(pNode),
                               m_nLoopLen(0), m_flIntegrator(0),
                               m_iLoopLoc(0),
                               m_flLoopMax(0), m_flLoopEnergy(0), m_flLoopMean(0), m_flBeatThresh((FLOAT)INT_MAX),
                               m_flBeatOutputPrediction(0),
                               m_iBeatLoc(-1), m_iCandidateLoc(-1), m_nBeatLife(0), m_flBeatStrength(0)
{
    
}

CBDNodeTimingNet::~CBDNodeTimingNet()
{
    m_lstNet.clear();
}


HRESULT CBDNodeTimingNet::Initialize
( 
    INT32 nLoopLen
)
{
    //////
    // Timing Net
    // Create loop of specified length
    m_lstNet.clear();
    m_nLoopLen = nLoopLen;
    for( INT32 ii=0; ii < m_nLoopLen; ii++ )
    {
        m_lstNet.push_front( g_BDParams.flLoopInitValue );
    } 

    //////
    // BeatOutput
    m_iLoopLoc = LoopLength();

    return S_OK;
}



HRESULT CBDNodeTimingNet::ExecuteStep
( 
    FLOAT   flInput
)
{
    // Translate onset value to Sigmoid input value (may need fuzzy stuff here later)
    if( flInput < 0.05f )
    {
        flInput = ((0.05f - flInput)/0.05f) * -0.5f;
    }
    else
    {
        flInput = flInput - 0.05f;
        flInput = min(flInput, 1.0f);
    }

    ////////////////////////////////
    // Sigmoid Growth/Decay function
    ////////////////////////////////
    FLOAT flNewValue;
    FLOAT flOldValue = m_lstNet.front();

    flNewValue = flOldValue + flInput * flOldValue * (1 - flOldValue);
    flNewValue = max( flNewValue, g_BDParams.flLoopInitValue );
    flNewValue = min( flNewValue, g_BDParams.flLoopMaxValue );

    m_lstNet.pop_front();
    m_lstNet.push_back( flNewValue );   

    ////////////////////////////////////
    // Update Loop stats
    // Decrement loop count
    m_iLoopLoc--;
    if( m_iLoopLoc == 0 )
        m_iLoopLoc = LoopLength();

    UpdateLoopStats();

    //m_flBeatThresh = m_flLoopMean * (1-g_BDParams.flBeatOutputThresh) + m_flLoopMax * g_BDParams.flBeatOutputThresh;    
    //m_flBeatThresh = max(m_flBeatThresh, g_BDParams.flBeatOutputMinThresh );
    ////////////////////////////////////


    //////
    // Update leaky integrator
    // Int = alpha * Int + beta * input
    //m_flIntegrator = m_flIntDecay * m_flIntegrator + m_flIntGrow * flNewValue;
    
    //m_flIntegrator = m_flLoopEnergy;
    //m_flIntegrator = 0.9*m_flIntegrator + 0.1 * (m_flLoopMean*m_flLoopMean) * 50;
    m_flIntegrator = 0.9*m_flIntegrator + 0.1 * m_flLoopEnergy * 10;

    return S_OK;
}



void CBDNodeTimingNet::UpdateLoopStats()
{
    FLOAT flMean = 0, flEnergy =0, flMax = 0;

    TimingLoopList::iterator iter = m_lstNet.begin();
    for( ; iter != m_lstNet.end(); iter++ )
    {
        flMean += *iter;
        flEnergy += (*iter)*(*iter);
        if( *iter > flMax )
            flMax = *iter;
    }
    m_flLoopMean = flMean / LoopLength();
    m_flLoopEnergy = sqrt(flEnergy / LoopLength());
    m_flLoopMax = flMax;
}


///////////////////
// Beat Output Methods

// Generate a beat output pulse from this loop
void CBDNodeTimingNet::GenerateBeatOutput()
{
    m_flBeatOutputPrediction = 0;

    TimingLoopList::iterator iterBeat = ++m_lstNet.begin();

    /*
    // Beat Threshold Level Option

    // Front is the corresponding location to the current set of input samples
    // 2nd to front is the next beat output - the one we want to predict
    // (remember this is often called just before finishing the processing for a group of samples)

    // Check 2nd from front against its neighbours to ensure it is bigger (ie a peak exists)
    TimingLoopList::iterator iterOther = m_lstNet.begin();
    
    BOOL fPeak = (*iterBeat > *iterOther);
    iterOther++;
    iterOther++;
    fPeak = fPeak && (*iterBeat > *iterOther);

    // A peak is defined as a loop neuron with activation that is large that of both 
    // neurons on either side (ahead and behind)
    if( fPeak )
    {
        if( *iterBeat >= m_flBeatThresh )
            m_flBeatOutputPrediction = *iterBeat;
    }
    */

    
    // One Beat Output Option
    if( m_iBeatLoc == -1 )
    {
        // No beat location exists, assign maximum
        if( (*iterBeat == m_flLoopMax) && (*iterBeat > g_BDParams.flLoopInitValue) )
        {
            m_iBeatLoc = LoopLength();
            m_flBeatOutputPrediction = *iterBeat;
            m_flBeatStrength = *iterBeat;
            m_nBeatLife = 2;
        }
    }
    else
    {
        // Elapsed beat location?  Output beat
        m_iBeatLoc -= 1;

        if( m_iBeatLoc == 0 )
        {
            m_iBeatLoc = LoopLength();
            m_flBeatOutputPrediction = *iterBeat;
            m_flBeatStrength = *iterBeat;
            // Is this the max?  Yes, reset life, no decrement life
            if( *iterBeat < m_flLoopMax )
            {
                m_nBeatLife -= 1;
                if( m_nBeatLife <= 0 )
                {
                    m_iBeatLoc = -1;
                    // Track Performance - Count number of revaluations of beat location
                    if( m_pNode->m_fSelected )
                        m_pNode->m_nBeatReEvaluations++;

                }
            }
            else
            {
                m_nBeatLife = 2;
            }
        }
    }

    //m_flBeatOutputPrediction = *iterBeat;
}