// BDRealTimeStage.h: interface for the CBDRealTimeStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDREALTIMESTAGE_H__3B364512_2B81_400F_9ED2_82CBA8E5B18C__INCLUDED_)
#define AFX_BDREALTIMESTAGE_H__3B364512_2B81_400F_9ED2_82CBA8E5B18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBDRealTimeStage  
{
public:
	CBDRealTimeStage();
	virtual ~CBDRealTimeStage();

    HRESULT CreateBeatStream( CDataStream *pStrmIn,         // Onset stream, constant tempo
                              CDataStream *pStrmOut,        // Beat steram, constant tempo
                              CDataStream *pStrmTempo,      // Sampling period, constant tempo
                              CDataStream *pStrmBeatPeriod, // Winning loop period, constant tempo
                              CDataStream *pStrmBeatInfo    // Extra Info
                            );

};

#endif // !defined(AFX_BDREALTIMESTAGE_H__3B364512_2B81_400F_9ED2_82CBA8E5B18C__INCLUDED_)
