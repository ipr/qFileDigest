//////////////////////////////////
//
// CDigestFile : helper to share handling with MD5 and SHA1 for input/output of data
//
// Ilkka Prusi, 2010
//

#pragma once

#ifndef _DIGESTFILE_H_
#define _DIGESTFILE_H_

// note: should reverse inheritance-order..

// TODO: replace with memory mapped handling
//#include "MemoryMappedFile.h"

#include "BufferedFile.h"

class CDigestFile : protected CBufferedFile
{
public:
	CDigestFile(unsigned long ulChunkSize = 1024)
		: CBufferedFile(ulChunkSize)
	{
	}
	virtual ~CDigestFile(void)
	{
		CloseFile();
	}
	
	uint8_t *GetChunk() const
	{
		return CBufferedFile::GetChunk();
	}
	
	size_t GetChunkSize() const
	{
		return CBufferedFile::GetChunkSize();
	}

	bool OpenFile(const TCHAR *szFilename)
	{
		return CBufferedFile::OpenFile(szFilename);
	}
	
	bool ReadChunk()
	{
		return CBufferedFile::ReadChunk();
	}
};


/*

class CDigestFile : protected CMemoryMappedFile
{
protected:
	unsigned long m_ulChunkSize;
	size_t m_nReadOffset;
	
public:
	CDigestFile(unsigned long ulChunkSize = 1024)
		: CMemoryMappedFile()
		, m_ulChunkSize(ulChunkSize)
		, m_nReadOffset(0)
	{
	}
	virtual ~CDigestFile(void)
	{
		Destroy();
	}
	
	uint8_t *GetChunk()
	{
		uint8_t *pChunk = (uint8_t*)GetView();
		pChunk = (pChunk + m_nReadOffset);
		return pChunk;
	}
	
	size_t GetChunkSize()
	{
		int64_t i64Size = GetSize();
		i64Size = (i64Size - (int64_t)m_nReadOffset);
		if (i64Size > m_ulChunkSize)
		{
			return m_ulChunkSize;
		}
		else
		{
			return (size_t)i64Size;
		}
	}

	bool OpenFile(const TCHAR *szFilename)
	{
		return CMemoryMappedFile::Create(szFilename);
	}
	
	bool ReadChunk()
	{
		// first read
		if (m_nReadOffset < 0)
		{
			m_nReadOffset = 0;
			return true;
		}
		
		size_t nChunkSize = GetChunkSize();
		if (nChunkSize <= 0)
		{
			// no more data in file
			return false;
		}
		
		m_nReadOffset += nChunkSize;
		return true;
	}
};
*/

#endif // ifndef _DIGESTFILE_H_

