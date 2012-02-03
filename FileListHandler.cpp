///////////////////////////////////////////////
//
// (see notes in header file)
//
// Ilkka Prusi, 2010
//

#include "FileListHandler.h"

#include "MD5.h"
#include "sha1.h"
//#include "DigestFile.h"

#include "FiletimeHelper.h"

#include <QFile>

int64_t CFileListHandler::GetSize(WIN32_FIND_DATA &FindData)
{
	if (FindData.nFileSizeHigh == 0)
	{
		// just use lower part when sufficiently small file
		return FindData.nFileSizeLow;
	}
	
	// otherwise calculate full size of file
	int64_t i64MaxLow = MAXDWORD;
	i64MaxLow += 1;
	int64_t i64UpperPart = FindData.nFileSizeHigh * i64MaxLow;
	return (i64UpperPart + (int64_t)FindData.nFileSizeLow);
}

void CFileListHandler::KeepFileInfo(WIN32_FIND_DATA &FindData, CFileEntry *pEntry)
{
	// get size of file
	int64_t i64Size = GetSize(FindData);
	CFiletimeHelper LastWrite(FindData.ftLastWriteTime);

	// in case reprocessing path, check if file has changed
	//
	if (pEntry->m_bEntryProcessed == true)
	{
		if (pEntry->m_i64FileSize != i64Size
			|| pEntry->m_LastWrittenTo != LastWrite)
		{
			pEntry->m_bEntryProcessed = false;
		}
	}

	// keep for later in any case
	pEntry->m_i64FileSize = i64Size;
	pEntry->m_LastWrittenTo = LastWrite;
}

wstring CFileListHandler::GetFullPath(const wstring &szBasePath, const wstring &szAddPath) const
{
	wstring szTmpPath = szBasePath;
	
	// add folder separator if it is not yet included
	if (szTmpPath.at(szTmpPath.length() - 1) != _T('\\'))
	{
		szTmpPath += _T("\\");
	}
	szTmpPath += szAddPath;
	
	return szTmpPath;
}

bool CFileListHandler::HandleFileList(HANDLE &hFind, WIN32_FIND_DATA &FindData, const long lPathIndex, CProcessedFileData &ProcessedData)
{
	bool bRet = true;
	
	while (bRet == true)
	{
		// TODO: another place for wide/ascii string wrapper..
		wstring szName = FindData.cFileName;

		// skip "." or ".." folder (stupid MSDOS crap..)
		if (szName == _T(".") || szName == _T(".."))
		{
			if (FindNextFile(hFind, &FindData) == FALSE)
			{
				DWORD dwErr = ::GetLastError();
				if (dwErr = ERROR_NO_MORE_FILES)
				{
					// successful output (all handled)
					break;
				}
				// otherwise error case
				bRet = false;
			}
			continue;
		}

		// in case of sub-folder,
		// add to temp list for handling later:
		// skip for now
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			m_SubFolderList.push_back(szName);

			if (FindNextFile(hFind, &FindData) == FALSE)
			{
				DWORD dwErr = ::GetLastError();
				if (dwErr = ERROR_NO_MORE_FILES)
				{
					// successful output (all handled)
					break;
				}
				// otherwise error case
				bRet = false;
			}
			continue;
		}

		// keep file info
		CFileEntry *pEntry = ProcessedData.AddFoundFile(szName, lPathIndex);
		KeepFileInfo(FindData, pEntry);
		
		// continue with next file
		if (FindNextFile(hFind, &FindData) == FALSE)
		{
			DWORD dwErr = ::GetLastError();
			if (dwErr = ERROR_NO_MORE_FILES)
			{
				// successful output (all handled)
				break;
			}
			// otherwise error case
			bRet = false;
		}
	}

	return bRet;
}


bool CFileListHandler::ListFile(const long lPathIndex, const wstring &szFileAndPath, CProcessedFileData &ProcessedData)
{
	TCHAR szPath[ _MAX_PATH+1 ];	// win32:n rajoitusten mukaan
	WIN32_FIND_DATA FindData;
	
	if (szFileAndPath.length() >= _MAX_PATH)
	{
		return false;
	}

	::memset(szPath, 0, _MAX_PATH+1);
	// note: length() return "character" count and we need byte-count
	::memcpy(szPath, szFileAndPath.c_str(), szFileAndPath.length()*sizeof(TCHAR));

	// handle hakemistolistaan, lista saadaan vain haun tuloksella
	// haetaan kaikkia tiedostoja joten merkitään tähdellä
	// tuloslistaa parsitaan "riveittäin"
	HANDLE hFind = FindFirstFile((LPCTSTR)szPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	
	// process found files and folders
	bool bRet = HandleFileList(hFind, FindData, lPathIndex, ProcessedData);

	// we must close this handle first
	// before we can list sub-folders
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
	
	// note: not expecting sub-folders in this case..
	
	return bRet;
}

bool CFileListHandler::ListFiles(CProcessedFileData &ProcessedData)
{
	TCHAR szPath[ _MAX_PATH+1 ];	// win32:n rajoitusten mukaan
	WIN32_FIND_DATA FindData;
	const wstring szWildCard = _T("*"); // wildcard for search-string
	
	// keep path string in indexed list for easy&efficient caching, lookup etc.
	const long lPathIndex = ProcessedData.m_Paths.AddPath(m_szPath);

	if (m_szPath.length() >= _MAX_PATH)
	{
		return false;
	}

	// add folder separator if it is not yet included
	wstring szTmpFind = GetFullPath(m_szPath, szWildCard);
	
	::memset(szPath, 0, _MAX_PATH+1);
	// note: length() return "character" count and we need byte-count
	::memcpy(szPath, szTmpFind.c_str(), szTmpFind.length()*sizeof(TCHAR));

	// handle hakemistolistaan, lista saadaan vain haun tuloksella
	// haetaan kaikkia tiedostoja joten merkitään tähdellä
	// tuloslistaa parsitaan "riveittäin"
	HANDLE hFind = FindFirstFile((LPCTSTR)szPath, &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	
	// process found files and folders
	bool bRet = HandleFileList(hFind, FindData, lPathIndex, ProcessedData);

	// we must close this handle first
	// before we can list sub-folders
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}

	// if there was no error,
	// continue by handling found sub-folders now
	if (bRet == true
		&& m_SubFolderList.size() > 0)
	{
		tSubFolderList::iterator itSubList = m_SubFolderList.begin();
		tSubFolderList::iterator itSubListEnd = m_SubFolderList.end();

		while (itSubList != itSubListEnd)
		{
			// full path to next folder
			wstring szTmpPath = GetFullPath(m_szPath, (*itSubList));

			// temp-instance of us for listing
			CFileListHandler TmpHandler(szTmpPath);
			if (TmpHandler.ListFiles(ProcessedData) == false)
			{
				// error listing sub-folder
				// -> break and return error
				bRet = false;
				break;
			}

			++itSubList;
		}
	}

	return bRet;
}

//////// 


void CFileProcess::IdentifyFile(CFileEntry &Entry, const uint8_t *pBuffer, const size_t nLength) const
{
	if (nLength < 16)
	{
		// no useful information in file anyway
		return;
	}
	
	// determine type from header (if possible)
	// when at least header should be availalbe,
	// see if file-type can be determined from it
	Entry.m_FileType.DetermineFileType(pBuffer, nLength);

	if (nLength >= 16)
	{
		// encode header for later diagnostics
		// when enough data
		HexEncode(pBuffer, 16, Entry.m_szHeaderDump);
		AsciiDumpBin(pBuffer, 16, Entry.m_szHeaderAscii);
	}
}

wstring CFileProcess::GetFullPath(CPathList &PathList, CFileEntry &Entry) const
{
	if (Entry.m_szName.length() == 0)
	{
		// incomplete name -> cannot continue
		return wstring();
	}
	
	wstring szPath = PathList.GetPathByIndex(Entry.m_lPathIndex);
	if (szPath.length() == 0)
	{
		// unknown path -> cannot continue
		return wstring();
	}
	
	// path + name
	wstring szFullName(szPath);
	if (szFullName.at(szFullName.length() - 1) != _T('\\'))
	{
		szFullName += _T("\\");
	}
	szFullName += Entry.m_szName;
	
	return szFullName;
}

// digest is fixed-length array from calculation,
// length depends on digest (MD5/SHA1),
// create hex-encoded dump of it for display and storage
void CFileProcess::HexEncode(const unsigned char *pBuffer, const size_t nLen, wstring &szOutput) const
{
	if (pBuffer == NULL
		|| pBuffer == nullptr)
	{
		return;
	}
	
	//wchar_t hextable[] = _T("0123456789ABCDEF");
	TCHAR hextable[] = _T("0123456789ABCDEF");

	// reserve 2*iLen space in output buffer first
	// if output should be larger than reserved string-space
	// (should improve efficiency slightly..)
	//
	// check remaining capacity against to be added
	size_t nSize = szOutput.size();
	size_t nCapacity = szOutput.capacity();
	if ((nCapacity - nSize) < nLen*2)
	{
		// reserve larger string-buffer
		// to fix existing and to be added
		szOutput.reserve(nSize + (nLen*2));
	}
	
	// determine half-bytes of each byte 
	// and appropriate character representing value of it
	// for hex-encoded string
	for ( size_t y = 0; y < nLen; y++ )
	{
		unsigned char upper;
		unsigned char lower;

		upper = lower = pBuffer[y];

		lower = lower & 0xF;

		upper = upper >> 4;
		upper = upper & 0xF;

		// C++ STL string grows automatically so we just push new
		// characters at the end, same way with reserve().
		//
		szOutput += hextable[upper]; szOutput += hextable[lower];
	}
}

void CFileProcess::AsciiDumpBin(const unsigned char *pBuffer, const size_t nLen, wstring &szOutput) const
{
	if (pBuffer == NULL
		|| pBuffer == nullptr)
	{
		return;
	}

	// reserve iLen space in output buffer first
	// if output should be larger than reserved string-space
	// (should improve efficiency slightly..)
	//
	// check remaining capacity against to be added
	size_t nSize = szOutput.size();
	size_t nCapacity = szOutput.capacity();
	if ((nCapacity - nSize) < nLen)
	{
		// reserve larger string-buffer
		// to fix existing and to be added
		szOutput.reserve(nSize + nLen);
	}

	for (size_t i = 0; i < nLen; i++)
	{
		if (isalnum(pBuffer[i]) == 0)
		{
			// use dot where non-displayable character
			szOutput += '.';
		}
		else
		{
			// use actual character
			szOutput += (char)pBuffer[i];
		}
	}
}

bool CFileProcess::ProcessFile(CProcessedFileData &ProcessedData, CFileEntry &Entry)
{
    if (Entry.m_bEntryProcessed == true)
	{
		// already processed previously?
		// -> ignore second time?
		// alternate: cleanup and process again?
		
        Entry.m_szMd5.clear();
        Entry.m_szSha1.clear();
        Entry.m_szHeaderDump.clear();
        Entry.m_szHeaderAscii.clear();
        Entry.m_bEntryProcessed = false;
	}
	
    if (Entry.m_i64FileSize == 0)
	{
		// empty file -> nothing to do
		return false;
	}
    if (Entry.m_szName.length() == 0)
	{
		// incomplete name -> cannot continue
		return false;
	}
	
	// path + name
    wstring szFullName = GetFullPath(ProcessedData.m_Paths, Entry);
	if (szFullName.length() == 0)
	{
		// unknown path -> cannot continue
		return false;
	}

	// digest checksum 
	// (both MD5 and SHA-1 in case of collisions of non-identical files
	// and in case of bugs in code..)
	CMD5 MyMD5;
	CSHA1 MySHA1;

	// use file size as chunk-size if smaller than 1MB,
	// otherwise use 1kB chunks:
	// this way we small files can be read entirely to RAM first
	// and we still support reading in smaller chunks for huge files
	//
    qint64 chunkSize = 1024;
    if (Entry.m_i64FileSize <= 1024*1024)
	{
        // for small files, use whole file
        chunkSize = Entry.m_i64FileSize;
	}

    QFile reader(QString::fromStdWString(szFullName));
    if (reader.open(QFile::ReadOnly) == false)
    {
        return false;
    }

    qint64 fileSize = reader.size();
    uchar *pView = reader.map(0, fileSize);
    if (pView == NULL)
    {
        return false;
    }
	
	// identify fileformat (if possible)
	//
    IdentifyFile(Entry, pView, fileSize);
	
	// proceed reading file and digesting checksums
	//
    qint64 offset = 0;
    while (offset < Entry.m_i64FileSize)
	{
        uchar *pPos = pView + offset;
        qint64 nextChunkSize = chunkSize; 
        if ((Entry.m_i64FileSize - offset) < chunkSize)
        {
			nextChunkSize = (Entry.m_i64FileSize - offset);
        }

		// pass current data (buffer) and count of bytes read
        // for processing into checksum/hash:
        // use same page from file in both before moving to next
        // to minimize IO and buffers needed
        //
        MyMD5.AddInput(pPos, nextChunkSize);
        MySHA1.AddInput(pPos, nextChunkSize);

        offset += nextChunkSize;
	}
	
	HexEncode(MyMD5.GetResultDigest(), 
			  MyMD5.GetDigestLength(), 
              Entry.m_szMd5);
	
	HexEncode(MySHA1.GetResultDigest(), 
			  MySHA1.GetDigestLength(), 
              Entry.m_szSha1);

    Entry.m_bEntryProcessed = true;

    // (avoid some miscompiles.. fix use later)
    CFileEntry *pEntry = &Entry;
	ProcessedData.AddHashOfEntry(pEntry);

	return true;
}



