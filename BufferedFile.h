//////////////////////////////////
//
// CBufferedFile : buffered C-style file-IO
//
// Ilkka Prusi, 2010
//


#ifndef BUFFEREDFILE_H
#define BUFFEREDFILE_H

#include <stdint.h>

#include <tchar.h>

#include <stdio.h>


class CBufferedFile
{
protected:
	FILE *m_pFile; // file

	unsigned long m_ulBufSize; // size of buffer
	uint8_t *m_pBuffer; // temp buffer for data to digest

	size_t m_nReadBytes;
	
public:
	
	CBufferedFile(unsigned long ulChunkSize = 1024)
		: m_pFile(NULL)
		, m_pBuffer(0)
		, m_ulBufSize(0)
		, m_nReadBytes(0)
	{
		if (ulChunkSize > 0)
		{
			m_pBuffer = new uint8_t[ulChunkSize];
			m_ulBufSize = ulChunkSize;
		}
	}
	
	virtual ~CBufferedFile(void)
	{
		CloseFile();
		
		if (m_pBuffer != NULL)
		{
			delete m_pBuffer;
			m_pBuffer = NULL;
		}
	}

	uint8_t *GetChunk() const
	{
		return m_pBuffer;
	}
	
	size_t GetChunkSize() const
	{
		return m_nReadBytes;
	}

	bool OpenFile(const TCHAR *szFilename);
	
	void CloseFile();

	bool ReadChunk();
	
	/*
	size_t GetSizeOfFile(void)
	{
		size_t nSize = 0;
		
		fseek(m_pFile, 0L, SEEK_END);
		nSize = ftell(m_pFile);
		
		fseek(m_pFile, 0L, SEEK_SET);
		
		return nSize;
	}
	
	static size_t GetSizeOfFile(const TCHAR *szFilename)
	{
		struct _stat stFileStatus;
		int iRet = _tstat(szFilename, &stFileStatus);
		if (iRet >= 0)
		{
			return stFileStatust.st_size;
		}
		return 0;
	}
	*/
	
};

#endif // BUFFEREDFILE_H
