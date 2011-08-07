// MFileWriter.h: interface for the CMFileWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFILEWRITER_H__54042BB6_5840_4472_9322_4B709B8C6356__INCLUDED_)
#define AFX_MFILEWRITER_H__54042BB6_5840_4472_9322_4B709B8C6356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BDNode.h"
#include "BDNodeTimingNet.h"
#include <limits>

using namespace std;



template <class ListClass>
class CMFileWriter  
{
public:
	CMFileWriter();
	virtual ~CMFileWriter();

    BOOL    Open( LPCTSTR lpszFilename, BOOL fCreate );
    BOOL    Close();

    BOOL    WriteFloatArray( LPCTSTR lpszName, FLOAT *pArray, INT32 nLen );
    
    BOOL    WriteFloatList( LPCTSTR lpszName, ListClass List );

    BOOL    WriteTimingLoops( NodeList List );

protected:

    CFile   m_File;
};






template <class ListClass>
CMFileWriter<ListClass>::CMFileWriter()
{

}

template <class ListClass>
CMFileWriter<ListClass>::~CMFileWriter()
{

}

template <class ListClass>
BOOL CMFileWriter<ListClass>::Open
( 
    LPCTSTR lpszFilename,   
    BOOL fCreate 
)
{
    BOOL fSuccess;

    if( fCreate )
    {
        fSuccess = m_File.Open( lpszFilename, CFile::modeCreate | CFile::modeWrite );
    }
    else
    {
        fSuccess = m_File.Open( lpszFilename, CFile::modeWrite );
        if( fSuccess )
            m_File.SeekToEnd();
    }

    return fSuccess;
}

template <class ListClass>
BOOL CMFileWriter<ListClass>::Close()
{
    m_File.Close();
    return TRUE;
}

template <class ListClass>
BOOL CMFileWriter<ListClass>::WriteFloatArray
(
    LPCTSTR lpszName, 
    FLOAT *pArray, 
    INT32 nLen 
)
{
    TCHAR szBuffer[128];
    sprintf( szBuffer, "%s = [", lpszName );
    m_File.Write( szBuffer, strlen(szBuffer) );

    for( INT32 ii = 0; ii < nLen; ii++ )
    {   
        if( pArray[ii] == numeric_limits<float>::infinity() )
            sprintf( szBuffer, "%f, ", 0 );     //NAN, INF
        else
            sprintf( szBuffer, "%f, ", pArray[ii] );
        m_File.Write( szBuffer, strlen(szBuffer) );
    }

    m_File.Write( _T("];\n\n"), 4 );

    return TRUE;
}


template <class ListClass>
BOOL CMFileWriter<ListClass>::WriteFloatList
( 
    LPCTSTR lpszName, 
    ListClass List 
)
{
    TCHAR szBuffer[128];
    sprintf( szBuffer, "%s = [", lpszName );
    m_File.Write( szBuffer, strlen(szBuffer) );

    ListClass::iterator iter;
    for( iter = List.begin(); iter != List.end(); iter++ )
    {
        FLOAT flValue = *iter;
        sprintf( szBuffer, "%f, ", flValue );
        m_File.Write( szBuffer, strlen(szBuffer) );
    }

    m_File.Write( _T("];\n\n"), 4 );

    return TRUE;
}


template <class ListClass>
BOOL CMFileWriter<ListClass>::WriteTimingLoops
( 
    NodeList List 
)
{
    TCHAR szBuffer[128];
    TCHAR szBuffer2[128];

    NodeList::iterator iter;
    for( iter = List.begin(); iter != List.end(); iter++ )
    {
        sprintf( szBuffer, "Net_%d", (INT32)((*iter)->Period()*1000) );
        WriteFloatList( szBuffer, (*iter)->TimingNet()->Net() );
        // Matlab command - debug
        sprintf( szBuffer2, "%s_Mean = mean(%s.^2)\n", szBuffer, szBuffer );
        m_File.Write( szBuffer2, strlen(szBuffer2) );
    }

    return TRUE;
}




#endif // !defined(AFX_MFILEWRITER_H__54042BB6_5840_4472_9322_4B709B8C6356__INCLUDED_)
