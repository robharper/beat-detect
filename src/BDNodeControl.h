// BDTimingNetControl.h: interface for the CBDNodeControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDTIMINGNETCONTROL_H__599C3681_5B9D_45EB_B524_8D670CD74283__INCLUDED_)
#define AFX_BDTIMINGNETCONTROL_H__599C3681_5B9D_45EB_B524_8D670CD74283__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <list>
#include "BDNode.h"
#include "BDIOIStatCollector.h"



class CBDNodeControl  
{
public:
	CBDNodeControl();
	virtual ~CBDNodeControl();

    HRESULT     Initialize();
    HRESULT     ExecuteStep( FLOAT * pflInputBuffer, sIOIStats *pStats );

    CBDNode   * BestNode()                  { return m_pNodeBest; }

    NodeList        m_lstNodes;

protected:
    HRESULT     AddNode( FLOAT flNodePeriod );

    CBDNode *       m_pNodeBest;
    // Option 1 (also required for 2)
    FLOAT           m_flMaxCSN;
    // Option 2
    FLOAT           m_flMaxCSNTimeout;

};

#endif // !defined(AFX_BDTIMINGNETCONTROL_H__599C3681_5B9D_45EB_B524_8D670CD74283__INCLUDED_)
