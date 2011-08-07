// AudioStream.h: interface for the CPCMAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_AUDIOSTREAM_H_)
#define _AUDIOSTREAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAudioStream : public CDataStream
{
public:
    CAudioStream();
    virtual ~CAudioStream();
    
    HRESULT LoadFromWaveFile( LPCTSTR lpszFilename );
    HRESULT SaveToWaveFile( LPCTSTR lpszFilename );
        
protected:

    HRESULT LoadReadWaveData( HMMIO hmmioFile, LPMMCKINFO pckInRIFF );
    HRESULT SaveWriteWaveData( HMMIO hmmioFile );
};


#endif // _AUDIOSTREAM_H_
