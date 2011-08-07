// BDTimingNet.h: interface for the CBDNodeTimingNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDTIMINGNET_H__CEE77B31_8F62_4CE0_81EF_1F5FFF2DC3D8__INCLUDED_)
#define AFX_BDTIMINGNET_H__CEE77B31_8F62_4CE0_81EF_1F5FFF2DC3D8__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <list>

typedef std::list<FLOAT> TimingLoopList;

class   CBDNode;


class CBDNodeTimingNet  
{
public:
	CBDNodeTimingNet( CBDNode * pNode );
	virtual ~CBDNodeTimingNet();

    HRESULT     Initialize( INT32 nLoopLen );
    
    // Timing Net Execution
    HRESULT     ExecuteStep( FLOAT flInput );

    // Beat Output Methods
    FLOAT       BeatOutputPrediction()      { return m_flBeatOutputPrediction; }
    BOOL        LoopComplete()              { return (m_iLoopLoc==LoopLength()); }

    // Accessors
    FLOAT           NetEnergy() const       { return m_flIntegrator; }
    INT32           LoopLength() const      { return m_nLoopLen; }

    // Semi-Public
    TimingLoopList  Net()                   { return m_lstNet; }
    void            GenerateBeatOutput();

    FLOAT               m_flBeatStrength;

protected:
    
    void            UpdateLoopStats();

    // Timing Net
    TimingLoopList      m_lstNet;
    INT32               m_nLoopLen;

    // Integrator
    FLOAT               m_flIntegrator;

    // Beat Output   
    INT32               m_iLoopLoc;
    FLOAT               m_flBeatThresh;
    FLOAT               m_flLoopMean;
    FLOAT               m_flLoopEnergy;
    FLOAT               m_flLoopMax;

    FLOAT               m_flBeatOutputPrediction;

    // Beat Output Version 2
    INT32               m_iBeatLoc;
    INT32               m_iCandidateLoc;
    INT32               m_nBeatLife;
    

    CBDNode           * m_pNode;
    
};

#endif // !defined(AFX_BDTIMINGNET_H__CEE77B31_8F62_4CE0_81EF_1F5FFF2DC3D8__INCLUDED_)
