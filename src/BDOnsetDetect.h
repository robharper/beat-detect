// BDOnsetDetect.h: interface for the CBDOnsetDetect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDONSETDETECT_H__DBB5F21C_EB79_4C07_8780_C0B523BD5581__INCLUDED_)
#define AFX_BDONSETDETECT_H__DBB5F21C_EB79_4C07_8780_C0B523BD5581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBDOnsetDetect  
{
public:
	CBDOnsetDetect();
	virtual ~CBDOnsetDetect();

    HRESULT CreateOnsetStream( CAudioStream *pStrmIn, CDataStream *pStrmOut, CDataStream *pStrmInternal );

protected:
	HRESULT ThresholdStream( CDataStream *pStrmIn, CDataStream *pStrmEnv, CDataStream *pStrmOut );
	
	HRESULT ProcessEnvelope( CDataStream *pStrmIn, CDataStream *pStrmOut );

};

#endif // !defined(AFX_BDONSETDETECT_H__DBB5F21C_EB79_4C07_8780_C0B523BD5581__INCLUDED_)
