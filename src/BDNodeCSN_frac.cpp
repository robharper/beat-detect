// BDNodeCSN.cpp: implementation of the CBDNodeCSN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BeatDetect.h"
#include "BDNodeTimingNet.h"
#include "BDNodeCSN.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBDNodeCSN::CBDNodeCSN( CBDNode * pNode ) : m_pNode(pNode)
{

}

CBDNodeCSN::~CBDNodeCSN()
{
    m_lstLinks.clear();
}


HRESULT CBDNodeCSN::Initialize()
{
    //////
    // CSN
    m_lstLinks.clear();

    m_flCSNOutput = g_BDParams.flCSNMinAct;
    m_flCSNOutputNew = g_BDParams.flCSNMinAct;

    return S_OK;
}   


/////////////
//CSN Methods
// Add specified net to link list and calculate link weight
HRESULT CBDNodeCSN::AddCSNLink
( 
    CBDNode * pNode
)
{
    /*
    if( 1 ) //pNet->LoopLength() < LoopLength() )
    {
        // The loop to add is shorter and therefore can affect this loop's activation

        // Formula = 
        // Dif = 0.5 - abs((LA/LB mod 1)-0.5)
        // Weight = (Max-Min)*((1-2*Dif)^Alpha) + Min
        FLOAT flLBig = max( pNet->LoopLength(), LoopLength() );
        FLOAT flLSmall = min( pNet->LoopLength(), LoopLength() );

        FLOAT flDif = 0.5 - fabs( fmod(flLBig/flLSmall, 1) - 0.5 );
    
        LinkInfo.flLink = (g_BDParams.flCSNMaxLink-g_BDParams.flCSNMinLink) * pow(1-2*flDif, g_BDParams.flCSNAlpha) 
                          + g_BDParams.flCSNMinLink;
    }
    else
    {
        // The loop to add is longer than this and therefore cannot affect this loop's activation
        LinkInfo.flLink = 0;
    }
    */

    m_lstLinks.push_back( pNode );

    return S_OK;
}

// Remove specified net from link list
HRESULT CBDNodeCSN::RemoveCSNLink
( 
    CBDNode * pNode
)
{
    HRESULT hr = E_FAIL;

    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        if( (*iter) == pNode )
        {
            // Found the one to remove
            m_lstLinks.erase( iter );
            hr = S_OK;
            break;
        }
    }

    return hr;
}

// Tell all linked nets to remove this net from their lists
HRESULT CBDNodeCSN::FlushCSNLinks()
{
    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        // Remove this Netlist from all other nets' links
        (*iter)->CSN()->RemoveCSNLink( m_pNode );
    }

    return S_OK;
}


HRESULT CBDNodeCSN::UpdateCSN
(
    FLOAT flNetEnergy
)
{
    //////
    // Decay the activation
    m_flCSNOutputNew = m_flCSNOutput * g_BDParams.flCSNDecay;

    //////
    // Calculate the linked weighting contributions
    FLOAT flCSNChange = 0;
    for( NodeList::iterator iter = m_lstLinks.begin(); iter != m_lstLinks.end(); iter++ )
    {
        //if( ((*iter)->CSNOutput() > 0) /*&& ((*iter)->Period() < m_pNode->Period())*/ )
        {
            // Calculate link strength
            FLOAT flLBig = max( (*iter)->Period(), m_pNode->Period() );
            FLOAT flLSmall = min( (*iter)->Period(), m_pNode->Period() );

            // Say we have 24 and 100, we want to find 100/96 (ie the closest integer multiple
            // 24 can make near 100
            //FLOAT flFractionalDif = flLBig / (((INT32)(flLBig/flLSmall + 0.5f))*flLSmall);
            FLOAT flFractionalDif = flLBig / flLSmall;
            // Modulo 1 : [-0.5, 0.5]
            FLOAT flDif = 0.5 - fabs( fmod(flFractionalDif, 1) - 0.5 );
            // Calculate function of modulo where flDif should result in link of about 1
            // and flDif >> 0 or flDif << 0 should result in link of about -1
            FLOAT flLink = (g_BDParams.flCSNMaxLink-g_BDParams.flCSNMinLink) * pow(1-2*flDif, g_BDParams.flCSNAlpha) 
                              + g_BDParams.flCSNMinLink;

            // Calculate the effect of this node on the current node (only positive nodes affect)
            //flCSNChange += flLink * (*iter)->CSNOutput();
            flCSNChange += flLink * (*iter)->TimingNet()->NetEnergy();
        }
    }
    // Add the leaky integrator input to the change
    flCSNChange += flNetEnergy * g_BDParams.flCSNInputLink;

    //////
    // Weight the activation change by the distance between current value and
    // the max or min allowed activation level
    if( flCSNChange > 0 )
        flCSNChange *= g_BDParams.flCSNMaxAct - m_flCSNOutput;
    else
        flCSNChange *= m_flCSNOutput - g_BDParams.flCSNMinAct;

    //////
    // Calculate new CSN unit activation
    m_flCSNOutputNew = BOUND( m_flCSNOutputNew + flCSNChange, g_BDParams.flCSNMinAct, g_BDParams.flCSNMaxAct );

    return S_OK;
}


HRESULT CBDNodeCSN::CommitCSN()
{
    m_flCSNOutput = m_flCSNOutputNew;
    return S_OK;
}