
#ifndef _ANSIFILE_H_
#define _ANSIFILE_H_

#include <string>

// ANSI-C style file-API helper
class CAnsiFile
{
protected:
	FILE *m_pFile;
	size_t m_nFileSize;
	bool m_bIsWritable;

	size_t GetSizeOfFile()
	{
		if (m_pFile == NULL)
		{
			return 0;
		}

		// should be at start (0)
		long lCurrent = ftell(m_pFile);

		// locate end
		if (fseek(m_pFile, 0L, SEEK_END) != 0)
		{
			return 0;
		}
		size_t nSize = ftell(m_pFile);

		// return to start
		fseek(m_pFile, lCurrent, SEEK_SET);
		return nSize;
	}

public:
	CAnsiFile(void)
		: m_pFile(NULL)
		, m_nFileSize(0)
		, m_bIsWritable(false)
	{}
	CAnsiFile(const std::string &szFile, bool bWritable = false)
		: m_pFile(NULL)
		, m_nFileSize(0)
		, m_bIsWritable(bWritable)
	{
		Open(szFile, bWritable);
	}
	~CAnsiFile(void)
	{
		Close();
	}

	operator FILE *() const
	{
		return m_pFile;
	}

	bool Open(const std::string &szFile, bool bWritable = false)
	{
		Close(); // close previous (if any)

		if (bWritable == false)
		{
			m_pFile = fopen(szFile.c_str(), "rb");
			m_nFileSize = GetSizeOfFile();
		}
		else
		{
			// size zero if new file..
			m_pFile = fopen(szFile.c_str(), "wb");
		}
		m_bIsWritable = bWritable;
		return IsOk();
	}

	void Close()
	{
		if (m_pFile != NULL)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
	}

	size_t GetSize()
	{
		// get size as it was in start
		if (m_bIsWritable == false)
		{
			return m_nFileSize;
		}
		// determine current size
		return GetSizeOfFile();
	}

	bool IsOk()
	{
		if (m_pFile == NULL)
		{
			return false;
		}

		if (ferror(m_pFile) != 0) 
		{
			return false;
		}
		return true;
	}

	bool Flush()
	{
		if (fflush(m_pFile) != 0)
		{
			return false;
		}
		return true;
	}

	bool Write(const void *pBuffer, const size_t nBytes)
	{
		size_t nWritten = fwrite(pBuffer, 1, nBytes, m_pFile);
		if (IsOk() == false
			|| nWritten < nBytes)
		{
			return false;
		}
		return Flush();
	}

	bool Read(void *pBuffer, const size_t nBytes)
	{
		size_t nRead = fread(pBuffer, 1, nBytes, m_pFile);
		if (IsOk() == false
			|| nRead < nBytes)
		{
			return false;
		}
		return true;
	}
};


#endif // ifndef _ANSIFILE_H_

