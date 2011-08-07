// BDIOIStatCollector.h: interface for the CBDIOIStatCollector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDIOISTATCOLLECTOR_H__D01C4BC1_F253_4317_9E48_36E210881AE4__INCLUDED_)
#define AFX_BDIOISTATCOLLECTOR_H__D01C4BC1_F253_4317_9E48_36E210881AE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define IOISTATS_PARZEN_HALF_WINDOW_SIZE     5
#define IOISTATS_PARZEN_WINDOW_SIZE     (IOISTATS_PARZEN_HALF_WINDOW_SIZE * 2 + 1)


#include <list>


/////////////////////////////////////////////
typedef struct
{
    FLOAT           flPeriod;
    INT32           nRefCount;
    FLOAT           flHighestEnergy;
} sIOIPeriodEntry;


typedef std::list<sIOIPeriodEntry> IOIPeriodsList;
typedef std::list<INT32> OnsetList;

#define IOISTATS_HISTLEN    2000

typedef struct
{
    FLOAT           aflIOIHist[IOISTATS_HISTLEN];
    
    IOIPeriodsList  lstDominantIOI;
} sIOIStats;
/////////////////////////////////////////////




class CBDIOIStatCollector  
{
public:
	CBDIOIStatCollector();
	virtual ~CBDIOIStatCollector();

    HRESULT     Initialize( sIOIStats * pStats );     

    HRESULT     ExecuteStep( FLOAT flSample, sIOIStats * pStats );

protected:
    HRESULT     FindDominantIOIs( FLOAT flPeriod, sIOIStats * pStats );

    OnsetList   m_lstOnset;

    INT32       m_nLastOnsetDelay;
    INT32       m_nMaxIOI;

    FLOAT       m_aflParzenWindow[IOISTATS_PARZEN_WINDOW_SIZE];

};

#endif // !defined(AFX_BDIOISTATCOLLECTOR_H__D01C4BC1_F253_4317_9E48_36E210881AE4__INCLUDED_)
