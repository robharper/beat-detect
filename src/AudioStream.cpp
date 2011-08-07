// AudioStream.cpp: implementation of the CAudioStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h> 
#include "DataStream.h"
#include "AudioStream.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioStream::CAudioStream()
{
}

CAudioStream::~CAudioStream()
{
}


HRESULT CAudioStream::LoadFromWaveFile
( 
  LPCTSTR lpszFilename
)
{
    HRESULT hr = S_OK;

    // Cannot load wave file if memory already allocated for another wave
    if( NULL != m_pData )
        return E_FAIL;

    HMMIO hmmioFile = mmioOpen( (LPTSTR)lpszFilename, NULL, MMIO_ALLOCBUF | MMIO_READ );
    if( hmmioFile == NULL )
        return E_FAIL;

    // Read main RIFF
    MMCKINFO ckInRIFF;
    if( mmioDescend(hmmioFile, &ckInRIFF, NULL, 0) == 0 ) 
    {
        // Check Type
        if( (ckInRIFF.ckid == FOURCC_RIFF) &&
            (ckInRIFF.fccType == mmioFOURCC('W', 'A', 'V', 'E')) ) 
        {
            //
            // Descend into format chunk
            //
            MMCKINFO ckIn;
            ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');     
            if( mmioDescend(hmmioFile, &ckIn, &ckInRIFF, MMIO_FINDCHUNK) == 0 ) 
            {
                // Read format
                PCMWAVEFORMAT   pcmWaveFormat;
                if( mmioRead(hmmioFile, (HPSTR) &pcmWaveFormat, (long) sizeof(pcmWaveFormat)) == 
                    (long) sizeof(pcmWaveFormat) )  
                {
                    //
                    // Fill in Wave Data
                    //
                    m_nBitsPerSample = pcmWaveFormat.wBitsPerSample;
                    m_nSampleRate = pcmWaveFormat.wf.nSamplesPerSec;
                    m_nChannels = pcmWaveFormat.wf.nChannels;
                    m_fNormalized = FALSE;

                    if( mmioAscend(hmmioFile, &ckIn, 0) == 0 ) 
                    {
                        // No errors
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }
                else
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                hr = E_FAIL;
            }

            //
            // Success, read data
            //
            if( SUCCEEDED(hr) )
            {
                hr = LoadReadWaveData( hmmioFile, &ckInRIFF );
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }

    // Close File
    if (hmmioFile != NULL) 
    { 
        mmioClose(hmmioFile, 0); 
        hmmioFile = NULL; 
    }

    return hr;
}


HRESULT CAudioStream::LoadReadWaveData
( 
  HMMIO hmmioFile, 
  LPMMCKINFO pckInRIFF
)
{
    HRESULT hr = S_OK;

    // Descend into data chunk
    MMCKINFO ckIn;
    ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');     
    if( mmioDescend(hmmioFile, &ckIn, pckInRIFF, MMIO_FINDCHUNK) == 0 ) 
    {
        MMIOINFO    mmioinfoIn;
        if( mmioGetInfo(hmmioFile, &mmioinfoIn, 0) == 0) 
        {
            // Allocate Buffer
            m_pData = new BYTE[ckIn.cksize];
            if( m_pData != NULL )
            {
                // Copy all the data bytes
                for (unsigned int cT = 0; cT < ckIn.cksize; cT++) 
                { 
                    if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) 
                    {     
                        if ( mmioAdvance(hmmioFile, &mmioinfoIn, MMIO_READ) != 0) 
                        {
                            hr = E_FAIL;
                            break;
                        }
                        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) 
                        {
                            hr = E_FAIL;
                            break;
                        }
                    }  
                    // Actual copy. 
                    *((BYTE*)m_pData+cT) = *((BYTE*)mmioinfoIn.pchNext);
                    (BYTE*)mmioinfoIn.pchNext++;
                }

                
                if( SUCCEEDED(hr) )
                {
                    // Calculate number of samples = numbytes / (bytespersample * numchannels)
                    m_nSamples = ckIn.cksize / ((m_nBitsPerSample/8) * m_nChannels);
                }
                else
                {
                    // Failed, delete buffer
                    delete[] m_pData;
                }
            }
            else
            { 
                hr = E_OUTOFMEMORY;
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}


HRESULT CAudioStream::SaveToWaveFile
( 
  LPCTSTR lpszFilename 
)
{
    HRESULT hr = S_OK;
    
    // If no data, cannot save
    if( NULL == m_pData )
        return E_FAIL;
    
    HMMIO hmmioFile = mmioOpen( (LPTSTR)lpszFilename, NULL, MMIO_ALLOCBUF | MMIO_WRITE | MMIO_CREATE );
    if( hmmioFile == NULL )
        return E_FAIL;
    
    MMCKINFO ckOutRiff;
    ckOutRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    ckOutRiff.cksize = 0;
    
    // Create main chunk
    if( mmioCreateChunk( hmmioFile, &ckOutRiff, MMIO_CREATERIFF ) != 0 )
    {
        hr = E_FAIL;
    }

    // Fill in format chunk
    if( SUCCEEDED(hr) )
    {
        MMCKINFO ckOut;
        ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');     
        ckOut.cksize = sizeof(PCMWAVEFORMAT);   
        if( mmioCreateChunk( hmmioFile, &ckOut, 0) == 0)
        {
            WAVEFORMATEX wfx;
            wfx.wFormatTag = WAVE_FORMAT_PCM;
            wfx.nChannels = GetNumChannels();
            wfx.nSamplesPerSec = GetSampleRate();
            wfx.nAvgBytesPerSec = GetNumChannels() * GetSampleRate() * GetBitsPerSample()/8;
            wfx.nBlockAlign = GetNumChannels() * GetBitsPerSample()/8;
            wfx.wBitsPerSample = GetBitsPerSample();
            if( mmioWrite( hmmioFile, (HPSTR)&wfx, sizeof(PCMWAVEFORMAT)) == sizeof(PCMWAVEFORMAT)) 
            {
                if( mmioAscend( hmmioFile, &ckOut, 0) != 0)  
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                hr = E_FAIL;
            }
        }
        else
        {
            hr = E_FAIL;
        }        
    }

    // Write wave file data chunk
    if( SUCCEEDED(hr) )
    {
        hr = SaveWriteWaveData( hmmioFile );
    }
        
    // Ascend to top of file
    if( SUCCEEDED(hr) )
    {
        if( mmioAscend( hmmioFile, &ckOutRiff, 0) != 0)
        {
            hr = E_FAIL;
        }
    }

    // Close the final file.
    if( hmmioFile != NULL )
    {
        mmioClose( hmmioFile, 0 ); 
        hmmioFile = NULL;
    }
    
    return hr;
}


HRESULT CAudioStream::SaveWriteWaveData
( 
  HMMIO hmmioFile
)
{
    HRESULT hr = S_OK;
    
    // Descend into data chunk
    MMCKINFO ckOut;
    ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
    ckOut.cksize = 0;     
    if( mmioCreateChunk(hmmioFile, &ckOut, 0) == 0) 
    {
        MMIOINFO    mmioinfoOut;
        if( mmioGetInfo(hmmioFile, &mmioinfoOut, 0) == 0) 
        {
            int cbWrite = GetNumChannels() * GetBitsPerSample()/8 * GetNumSamples();
            int cbActualWrite = 0;
            
            // Write wave data
            for (int cT=0; cT < cbWrite; cT++) 
            {        
                if (mmioinfoOut.pchNext == mmioinfoOut.pchEndWrite) 
                { 
                    mmioinfoOut.dwFlags |= MMIO_DIRTY; 
                    if( mmioAdvance(hmmioFile, &mmioinfoOut, MMIO_WRITE) != 0) 
                    {     
                        break; 
                    } 
                } 
                *((BYTE*)mmioinfoOut.pchNext) = *((BYTE*)m_pData+cT); 
                (BYTE*)mmioinfoOut.pchNext++;
                cbActualWrite++; 
            }     
            
            // Check wrote all we have to write
            if( cbActualWrite < cbWrite )
            {
                hr = E_FAIL;
            }

            mmioinfoOut.dwFlags |= MMIO_DIRTY;     
            if( mmioSetInfo(hmmioFile, &mmioinfoOut, 0) == 0) 
            {
                if( mmioAscend( hmmioFile, &ckOut, 0) != 0)
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                hr = E_FAIL;
            }
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else
    {
        hr = E_FAIL;
    }
    
    return hr;
}
