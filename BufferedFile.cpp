#include "BufferedFile.h"

#include <memory>

/*
#ifdef UNICODE
bool CBufferedFile::OpenFile(const wchar_t *szFilename)
#else
bool CBufferedFile::OpenFile(const char *szFilename)
#endif
*/

bool CBufferedFile::OpenFile(const TCHAR *szFilename)
{
	FILE *pFile = NULL;
	errno_t err;

	if (m_pBuffer != NULL)
	{
		::memset(m_pBuffer, 0, m_ulBufSize);
	}
	if (m_pFile != NULL)
	{
		CloseFile();
	}

/*	
#ifdef UNICODE
	err = _wfopen_s(&pFile, szFilename, L"rb");
#else	
	err = fopen_s(&pFile, szFilename, "rb");
#endif
	*/
	
	err = _tfopen_s(&pFile, szFilename, _T("rb"));
	if (pFile == NULL
		|| err != 0)
	{
		return false;
	}

	m_pFile = pFile;
	return true;
}

void CBufferedFile::CloseFile()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

bool CBufferedFile::ReadChunk()
{
	if (m_pFile == NULL)
	{
		return false;
	}
	if (m_pBuffer == NULL)
	{
		return false;
	}

	// get chunk of data and update file position
	m_nReadBytes = fread(m_pBuffer, 1, m_ulBufSize, m_pFile);

	// TODO: error?
	// feof or ferror

	return true;
}

