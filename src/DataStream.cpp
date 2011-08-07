// DataStream.cpp: implementation of the CDataStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataStream.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDataStream::CDataStream() : m_pData(NULL), m_nSamples(0), m_nChannels(1)
{
    
}

CDataStream::~CDataStream()
{
    // Clean up
    ReleaseData();
}


void CDataStream::ReleaseData()
{
    if( m_pData != NULL )
    {
        delete[] m_pData;
        m_pData = NULL;
        m_nSamples = 0;
    }
}


HRESULT CDataStream::CreateData
( 
    INT32 nBitsPerSample, 
    INT32 nSampleRate, 
    INT32 nSamples,
    BOOL  fNormalized
)
{
    if( IsValid() )
        return E_FAIL;

    if( (nBitsPerSample % 8) != 0 )
        return E_INVALIDARG;

    if( fNormalized && (nBitsPerSample < 32) )
        return E_INVALIDARG;

    m_pData = new BYTE[nSamples * (nBitsPerSample/8)];
    if( m_pData != NULL )
    {
        m_nBitsPerSample = nBitsPerSample;
        m_nSampleRate = nSampleRate;
        m_nSamples = nSamples;
        m_fNormalized = fNormalized;
        return S_OK;
    }

    return E_OUTOFMEMORY;
}


HRESULT CDataStream::CreateData
( 
    CDataStream const *pDataStreamCopyFrom 
)
{
    if( NULL != pDataStreamCopyFrom )
        return CreateData( pDataStreamCopyFrom->GetBitsPerSample(),
                           pDataStreamCopyFrom->GetSampleRate(), 
                           pDataStreamCopyFrom->GetNumSamples(),
                           pDataStreamCopyFrom->IsNormalized()
                         );
    else
        return E_INVALIDARG;
}


// Reallocate stream to new length - shorter or longer, pads with zeros
HRESULT CDataStream::Reallocate
(   
    INT32 nSamples 
)
{
    // Nothing to do
    if( nSamples == m_nSamples )
        return S_OK;

    HRESULT hr = E_FAIL;

    if( nSamples < m_nSamples )
    {
        // Request for shorter data length, just change the # of samples and be
        // done with it, no need for reallocation
        m_nSamples = nSamples;
        hr = S_OK;
    }
    else
    {
        LPVOID pData = new BYTE[nSamples * (m_nBitsPerSample/8)];
        if( pData != NULL )
        {
            // Copy over data and pad end with zeros - not quite efficient but who cares...
            memset( pData, 0, nSamples*(m_nBitsPerSample/8) );
            memcpy( pData, m_pData, m_nSamples*(m_nBitsPerSample/8) );
            
            delete[] m_pData;
            m_pData = pData;
            hr = S_OK;
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}


HRESULT CDataStream::Normalize()
{
    HRESULT hr = S_OK;

    if( IsNormalized() || !IsValid() )
        return E_FAIL;

    FLOAT * pflData = new FLOAT[GetNumSamples()];
    if( pflData != NULL )
    {
        BOOL fSuccess = FALSE;
        if( GetBitsPerSample() == 16 )
        {
            signed short * psData = (signed short *)GetData();
            // Copy over data and normalize
            for( INT32 ii=0; ii<GetNumSamples(); ii++ )
                pflData[ii] = (FLOAT)psData[ii] / 32768;

            fSuccess = TRUE;
        }
        else
        {
            // No support for other bit depths yet
            ASSERT( FALSE );
        }

        if( fSuccess )
        {
            delete[] m_pData;
            m_pData = pflData;
            m_fNormalized = TRUE;
            m_nBitsPerSample = sizeof(FLOAT)*8;
            hr = S_OK;
        }
        else
        {
            delete[] pflData;
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}


HRESULT CDataStream::DeNormalize
( 
    INT32 nBitsPerSample 
)
{
    HRESULT hr = S_OK;

    if( !IsNormalized() || !IsValid() )
        return E_FAIL;

    LPVOID pData = new BYTE[GetNumSamples() * (nBitsPerSample/8)];
    if( pData != NULL )
    {
        BOOL fSuccess = FALSE;
        if( nBitsPerSample == 16 )
        {
            signed short * psData = (signed short *)pData;
            // Copy over data and denormalize
            for( INT32 ii=0; ii<GetNumSamples(); ii++ )
                psData[ii] = (signed short)(((FLOAT *)GetData())[ii] * 32768);

            fSuccess = TRUE;
        }
        else
        {
            // No support for other bit depths yet
            ASSERT( FALSE );
        }
        
        if( fSuccess )
        {
            delete[] m_pData;
            m_pData = pData;
            m_fNormalized = FALSE;
            m_nBitsPerSample = nBitsPerSample;
            hr = S_OK;
        }
        else
        {
            delete[] pData;
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}