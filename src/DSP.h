// DSP.h: interface for the CDSP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSP_H__8A1F1D34_9930_437A_ACC1_4638DA9FFA6C__INCLUDED_)
#define AFX_DSP_H__8A1F1D34_9930_437A_ACC1_4638DA9FFA6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct
{
    DOUBLE dA[7];
    DOUBLE dB[7];
    INT32  nSamplesDelay;
} FILTER_6TH_COEFF;



class CDSP  
{
public:
	
	CDSP();
	virtual ~CDSP();

    // IIR Filter with A,B coefficients
    static  HRESULT DF2_Filter6( CDataStream* pStrmIn, CDataStream* pStrmOut, 
                                 FILTER_6TH_COEFF* pCoeff, BOOL fReverseGroupDelay = FALSE );
    
    static HRESULT RMSDecimate( CDataStream *pStrmIn, CDataStream *pStrmOut, INT32 nDec );
	
    static HRESULT Convolve( CDataStream *pStrmIn, CDataStream *pStrmOut, const FLOAT aflKernel[], const INT32 nKernelLen );

    static HRESULT Mix( CDataStream *pStrmIn1, FLOAT flVol1, CDataStream *pStrmIn2, FLOAT flVol2, CDataStream *pStrmOut );
 
};




#endif // !defined(AFX_DSP_H__8A1F1D34_9930_437A_ACC1_4638DA9FFA6C__INCLUDED_)
