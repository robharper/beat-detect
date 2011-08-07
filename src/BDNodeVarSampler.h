// BDNodeVarSampler.h: interface for the CBDNodeVarSampler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDNODEVARSAMPLER_H__7CE2ACC8_8A84_4B07_B0CD_BA395F50F377__INCLUDED_)
#define AFX_BDNODEVARSAMPLER_H__7CE2ACC8_8A84_4B07_B0CD_BA395F50F377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBDNode;


class CBDNodeVarSampler  
{
public:
	CBDNodeVarSampler( CBDNode * pNode );
	virtual ~CBDNodeVarSampler();

    HRESULT     Initialize( FLOAT flSamplerPeriod );

    HRESULT     ProcessInput( FLOAT * aflInputBuffer, BOOL * pfSampleComplete, FLOAT * pflSample );

    FLOAT &     SamplePeriod()             { return m_flPeriod; }
    FLOAT       IdealSamplePeriod()        { return m_flIdealPeriod; }
    FLOAT       IdealPeriodWeight()        { return m_flIdealPeriodWeight; }

    FLOAT       m_flOffset;

    // Testing
    FLOAT       flE;
    FLOAT       fldE;

protected:

    HRESULT     AdjustSamplingRate();

    FLOAT       m_flPeriod;

    // Sampling and Fuzzy Onsets
    FLOAT       m_flEndSam;
    FLOAT       m_flBeginSam;

    INT32       m_iCurSam;
    INT32       m_iLastOnset;
    FLOAT       m_flLastOnsetEnergy;
    FLOAT       m_flEnergyRemainder;

    // Sampling Rate Adjustment Variables
    FLOAT       m_flRecentBeat, m_flRecentOnset;
    BOOL        m_fFoundBeat;   
    FLOAT       m_flRecentBeatEnergy;
    FLOAT       m_flLastBeatTime;
    
    FLOAT       m_flIdealPeriodWeight;
    FLOAT       m_flLastExpWeight;

    // Ideal period according to the variable sampler
    FLOAT       m_flIdealPeriod;

    CBDNode *   m_pNode;
};

#endif // !defined(AFX_BDNODEVARSAMPLER_H__7CE2ACC8_8A84_4B07_B0CD_BA395F50F377__INCLUDED_)
