// BDOnsetStage.h: interface for the CBDOnsetStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDONSETSTAGE_H__17671C69_C882_4CB8_997E_CA3C04DAC347__INCLUDED_)
#define AFX_BDONSETSTAGE_H__17671C69_C882_4CB8_997E_CA3C04DAC347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define     NUM_BANDS       8  //9

class CBDOnsetStage  
{
public:
	CBDOnsetStage();
	virtual ~CBDOnsetStage();

    HRESULT CreateOnsetStream( CAudioStream *pStrmIn, CDataStream *pStrmOut, CDataStream *pStrmInternal );

protected:
    // Split the input signal into its separate frequency bands
    HRESULT BandSplitInput( CAudioStream *pStrmIn );
    // Reassemble onset streams into one output stream
    HRESULT ReassembleOnsets( CDataStream *pStrmOut );


protected:
    CAudioStream    m_aStrmBandInput[NUM_BANDS];
    CDataStream     m_aStrmOnset[NUM_BANDS];

};

#endif // !defined(AFX_BDONSETSTAGE_H__17671C69_C882_4CB8_997E_CA3C04DAC347__INCLUDED_)
