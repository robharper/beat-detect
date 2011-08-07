// BDNode.h: interface for the CBDNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDNODE_H__A2F21E93_2970_40D4_AE24_E779174D6335__INCLUDED_)
#define AFX_BDNODE_H__A2F21E93_2970_40D4_AE24_E779174D6335__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

class CBDNode;
class CBDNodeTimingNet;
class CBDNodeCSN;
class CBDNodeVarSampler;

typedef std::list<CBDNode *> NodeList;

class CBDNode  
{
public:
	CBDNode();
	virtual ~CBDNode();

    HRESULT     Initialize( FLOAT flPeriod );

    HRESULT     ExecuteStep( FLOAT * pflInputBuffer );
    HRESULT     CommitStep();

    void        SetIdealPeriod( FLOAT flIdealPeriod )       { m_flIdealPeriod = 0.5f*m_flIdealPeriod + 0.5f*flIdealPeriod; }
    HRESULT     AdjustPeriod();

    // Accessors per sae
    FLOAT       Period() const              { return m_flPeriod; }
    FLOAT       CSNOutput();
    FLOAT       BeatOutput();
    BOOL        LoopComplete();

    // Semi-Public
    CBDNodeTimingNet    *   TimingNet()     { return m_pNet; }
    CBDNodeCSN          *   CSN()           { return m_pCSN; }
    CBDNodeVarSampler   *   VarSampler()    { return m_pVarSampler; }

    FLOAT                   m_flIdealPeriod;

    /////////////////////////////////////////////////
    // Performance Measures
    HRESULT                 CalculatePerformanceMeasures();
    BOOL                    m_fSelected;
    FLOAT                   m_flSelectedTime;
    INT32                   m_nBeatReEvaluations;
    FLOAT                   m_flAvgPeriod;
    FLOAT                   m_flPredictionError;
    INT32                   m_nSelectedBeats;
    /////////////////////////////////////////////////

protected:
    CBDNodeTimingNet    *   m_pNet;
    CBDNodeCSN          *   m_pCSN;
    CBDNodeVarSampler   *   m_pVarSampler;

    FLOAT                   m_flPeriod;
    FLOAT                   m_flPreviousBeatStrength;
    
};

#endif // !defined(AFX_BDNODE_H__A2F21E93_2970_40D4_AE24_E779174D6335__INCLUDED_)
