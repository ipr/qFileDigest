//////////////////////////////////
//
// filelist handling for recursive listing and processing files,
// collection information such as MD5&SHA1 digest, filetype (by header) etc.
//
// Ilkka Prusi, 2010
//

#pragma once

#include <stdint.h>

#include <tchar.h>
#include <Windows.h>

#include <string>
//#include <wstring>
#include <map>
#include <set>
#include <vector>
using namespace std;

#include "FileType.h"
#include "FiletimeHelper.h"


// fwd. decl.
class CFileEntry;
class CFileEntry
{
public:
	// name of the file only
	wstring m_szName;

	// index to path list
	long m_lPathIndex;

	// size of file according to OS
	int64_t m_i64FileSize;

	// file type information
	CFileType m_FileType;
	
	// last modification time of file
	// (FILETIME in single integer)
	//
	//uint64_t m_u64LastWrittenTo;
	CFiletimeHelper m_LastWrittenTo;
	
	wstring m_szHeaderDump; // if type is not known..
	wstring m_szHeaderAscii; // if type is not known..

	wstring m_szMd5;  // MD5 hash-digest of file data
	wstring m_szSha1; // SHA-1 hash-digest of file data
	
	// if this entry has been processed yet
	bool m_bEntryProcessed;

	// in case file is within archive:
	// parent is the archive from which file is
	// extracted/decompressed for hashing
	CFileEntry *m_pParentEntry;
	
public:
	CFileEntry() 
		: m_szName()
		, m_lPathIndex(0)
		, m_i64FileSize(0)
		, m_FileType()
		, m_LastWrittenTo()
		, m_szHeaderDump()
		, m_szHeaderAscii()
		, m_szMd5()
		, m_szSha1()
		, m_bEntryProcessed(false)
		, m_pParentEntry(nullptr)
	{};

	CFileEntry(const wstring &szName, const long lPathIndex) 
		: m_szName(szName)
		, m_lPathIndex(lPathIndex)
		, m_i64FileSize(0)
		, m_FileType()
		, m_LastWrittenTo()
		, m_szHeaderDump()
		, m_szHeaderAscii()
		, m_szMd5()
		, m_szSha1()
		, m_bEntryProcessed(false)
		, m_pParentEntry(nullptr)
	{};
	
	~CFileEntry() 
	{};
	
	/*
	CFileEntry& operator=(const CFileEntry &Entry)
	{
		if (&Entry == this)
		{
			return *this;
		}
	};
	*/
	
	// same instance: same name and same folder,
	// filesystem usually does not allow more than one file
	// with given name to exist in the same folder at same time
	// -> same instance of file.
	//
	bool IsSameInstance(const CFileEntry &Entry)
	{
		if (m_lPathIndex == Entry.m_lPathIndex
			&& m_szName == Entry.m_szName)
		{
			// same instance (according to filesystem limitations)
			return true;
		}
		return false;
	}
	
	// identical (may be duplicate):
	// either hash-digest or both are equal
	// on both instances -> data is equal
	// -> identical (duplicate) file.
	// path and name do not matter in this case.
	//
	bool IsIdentical(const CFileEntry &Entry)
	{
		if (m_bEntryProcessed == false
			|| Entry.m_bEntryProcessed == false)
		{
			// either is not processed yet?
			// -> cannot know if identical instances or not
			return false;
		}
		
		if (m_szMd5 == Entry.m_szMd5
			|| m_szSha1 == Entry.m_szSha1)
		{
			// either or both hashes are identical
			// -> should be identical files (duplicate)
			return true;
		}
		
		// no match
		return false;
	}

};

// reduce memory needs by keeping list of paths
// and index where referenced
class CPathList
{
protected:
	// pair<index, path>:
	// vector should be fast enough..
	typedef pair<long, wstring> tPath;
	typedef vector<tPath> tPathList;
	tPathList m_PathList;
	
	// TODO: lookup from paths to files? (file-counts etc.)

public:
	CPathList()
	{}
	~CPathList()
	{}

	wstring GetPathByIndex(const long lIndex) const
	{
		tPathList::const_iterator itPaths = m_PathList.begin();
		tPathList::const_iterator itPathsEnd = m_PathList.end();
		while (itPaths != itPathsEnd)
		{
			if (itPaths->first == lIndex)
			{
				return itPaths->second;
			}
			++itPaths;
		}

		// not found
		return wstring();
	};

	long AddPath(const wstring &szPath)
	{
		tPathList::iterator itPaths = m_PathList.begin();
		tPathList::iterator itPathsEnd = m_PathList.end();
		while (itPaths != itPathsEnd)
		{
			if (itPaths->second == szPath)
			{
				// found: give index
				return itPaths->first;
			}
			++itPaths;
		}

		// not found:
		// use count as new index and add new
		long lIndex = (long)m_PathList.size();
		m_PathList.push_back(tPath(lIndex, szPath));
		return lIndex;
	};
};

// indexed list of files matching hashed digest
// to locate matching (identical) files
//
class CHashMatchList
{
public:
	// list of entries where either MD5, SHA1 or both match
	// to a given entry
	//
	typedef set<CFileEntry*> tMatchList;
	
protected:
	class CMatchedHash
	{
	public:
		// in case of duplicate-files,
		// keep list which files have same checksum
		// (TODO: set instead of vector..?)
		typedef vector<CFileEntry*> tEntryList;
		tEntryList m_vEntryList;
		
		CMatchedHash() 
			: m_vEntryList()
		{};
		~CMatchedHash() 
		{
			// nothing more to destroy 
			// (keep just pointers here, instances elsewhere)
			m_vEntryList.clear();
		};
		
		void AddToList(CFileEntry *pEntry)
		{
			/*
			tEntryList::const_iterator it = m_vEntryList.begin();
			tEntryList::const_iterator itEnd = m_vEntryList.end();
			while (it != itEnd)
			{
				if ((*it) == pEntry)
				{
					// same entry already exists
					return;
				}
				++it;
			}
			*/
			m_vEntryList.push_back(pEntry);
		}
		
		void GetMatching(CFileEntry *pEntry, tMatchList &MatchList) const
		{
			tEntryList::const_iterator it = m_vEntryList.begin();
			tEntryList::const_iterator itEnd = m_vEntryList.end();
			while (it != itEnd)
			{
				// skip same file-entry as given
				// (don't list same entry twice in output)
				if ((*it) == pEntry)
				{
					++it;
					continue;
				}
				
				// add if does not exist yet
				// (in case both MD5 and SHA1 have match (as usual))
				tMatchList::iterator itList = MatchList.find((*it));
				if (itList == MatchList.end())
				{
					MatchList.insert(tMatchList::value_type((*it)));
				}
				
				++it;
			}
		}
	};
	
	
	// key: hash (MD5 or SHA1 or both?)
	// value: link (reference) to file(s)
	//
	// note: may have multiple files by same hash
	// in case of duplicate files
	// -> purpose of program is to locate those..
	//
	typedef map<wstring, CMatchedHash> tHashToFiles;
	tHashToFiles m_Md5ToFiles;
	tHashToFiles m_Sha1ToFiles;

	void AddFileEntry(CFileEntry *pEntry, wstring &szHash, tHashToFiles &Hashes)
	{
		// check if container for matching file(s) already exists
		tHashToFiles::iterator it = Hashes.find(szHash);
		if (it == Hashes.end())
		{
			// add container for file(s) matching this hash
			Hashes.insert(tHashToFiles::value_type(szHash, CMatchedHash()));
			it = Hashes.find(szHash);
		}
		
		// add file to hash-matches
		it->second.AddToList(pEntry);
	}

	void GetMatchList(CFileEntry *pEntry, wstring &szHash, tHashToFiles &Hashes, tMatchList &MatchList)
	{
		tHashToFiles::const_iterator it = Hashes.find(szHash);
		if (it != Hashes.end())
		{
			it->second.GetMatching(pEntry, MatchList);
		}
	}
	
public:
	CHashMatchList() 
		: m_Md5ToFiles()
		, m_Sha1ToFiles()
	{};
	~CHashMatchList() 
	{
		m_Md5ToFiles.clear();
		m_Sha1ToFiles.clear();
	}

	void AddFileEntry(CFileEntry *pEntry)
	{
		AddFileEntry(pEntry, pEntry->m_szMd5, m_Md5ToFiles);
		AddFileEntry(pEntry, pEntry->m_szSha1, m_Sha1ToFiles);
	}

	// this should only give each file once
	// when both MD5 and SHA1 are matching,
	// given file should not be in the returned list
	//
	void GetMatchList(CFileEntry *pEntry, tMatchList &MatchList)
	{
		GetMatchList(pEntry, pEntry->m_szMd5, m_Md5ToFiles, MatchList);
		GetMatchList(pEntry, pEntry->m_szSha1, m_Sha1ToFiles, MatchList);
	}
};

// simple container for all related data and links:
// - file to path
// - hash to other (identical) files
//
class CProcessedFileData
{
public:
	CPathList m_Paths;
	CHashMatchList m_Hashes;

	// TODO: keep file names indexed
	// and lookup by path index and file name index ?
	//typedef pair<long, long> tEntryLookup;
	//typedef map<tEntryLookup, CFileEntry*> tEntryList;
	
    typedef vector<CFileEntry*> tFileList;
	tFileList m_vFileList;
	
public:
	CProcessedFileData()
		: m_Paths()
		, m_Hashes()
		, m_vFileList()
	{};
	~CProcessedFileData()
	{
		tFileList::const_iterator it = m_vFileList.begin();
		tFileList::const_iterator itEnd = m_vFileList.end();
		while (it != itEnd)
		{
			// 
			CFileEntry *pEntry = (*it);
			delete pEntry;
		
			++it;
		}
		m_vFileList.clear();
	};
	
	//CFileEntry *AddFoundFile(wstring &szName, long lPathIndex, int64_t i64Size)
	CFileEntry *AddFoundFile(wstring &szName, long lPathIndex)
	{
		tFileList::const_iterator it = m_vFileList.begin();
		tFileList::const_iterator itEnd = m_vFileList.end();
		while (it != itEnd)
		{
			CFileEntry *pEntry = (*it);
			if (pEntry->m_lPathIndex == lPathIndex
				&& pEntry->m_szName == szName)
			{
				// same file already exists
				// (cannot have two files with same name in same folder)
				return pEntry;
			}
			++it;
		}
		
		CFileEntry *pNewEntry = new CFileEntry(szName, lPathIndex);
		m_vFileList.push_back(pNewEntry);
		return pNewEntry;
	}
	
	void AddHashOfEntry(CFileEntry *pEntry)
	{
		m_Hashes.AddFileEntry(pEntry);
	}
	
	CHashMatchList::tMatchList GetMatchListOfEntry(CFileEntry *pEntry)
	{
		CHashMatchList::tMatchList MatchList;
		m_Hashes.GetMatchList(pEntry, MatchList);
		return MatchList;
	}
	
	size_t GetFileCount()
	{
		return m_vFileList.size();
	}
};


// recursive path-handler
// to list files and get metadata
//
class CFileListHandler
{
protected:
	wstring m_szPath;
	typedef vector<wstring> tSubFolderList;
	tSubFolderList m_SubFolderList;

	inline int64_t GetSize(WIN32_FIND_DATA &FindData);
	inline void KeepFileInfo(WIN32_FIND_DATA &FindData, CFileEntry *pEntry);
	inline wstring GetFullPath(const wstring &szBasePath, const wstring &szAddPath) const;

	bool HandleFileList(HANDLE &hFind, WIN32_FIND_DATA &FindData, const long lPathIndex, CProcessedFileData &ProcessedData);
	
public:
	CFileListHandler(wstring &szPath)
		: m_szPath(szPath)
	{}
	~CFileListHandler(void)
	{}
	
	bool ListFile(const long lPathIndex, const wstring &szFileAndPath, CProcessedFileData &ProcessedData);
	bool ListFiles(CProcessedFileData &ProcessedData);
};

// mostly just interface to handle a file
// (digest checksum, determine type etc.)
//
class CFileProcess
{
private:
	// internal counters for statistics of processed files

	// count of files processed
	uint64_t m_u64FilesProcessed;
	
	// TODO: amount of bytes in each file..
	// can wraparound , how should we count?
	uint64_t m_u64BytesProcessed;

	// entire processing time counting
	//FILETIME m_ftProcStart;
	//FILETIME m_ftProcEnd;
	CFiletimeHelper m_ProcStart;
	CFiletimeHelper m_ProcEnd;
	
	// per-file time counting
	//FILETIME m_ftFileStart;
	//FILETIME m_ftFileEnd;
	
	//wstring m_szRootPath;
	//CFileListHandler m_RootPath;

	// actual file&path processing and related data
	//CProcessedFileData m_FileData;

	
protected:

	void IdentifyFile(CFileEntry &Entry, const uint8_t *pBuffer, const size_t nLength) const;
	wstring GetFullPath(CPathList &PathList, CFileEntry &Entry) const;

	void HexEncode(const unsigned char *pBuffer, const size_t nLen, wstring &szOutput) const;
	void AsciiDumpBin(const unsigned char *pBuffer, const size_t nLen, wstring &szOutput) const;
	
    bool ProcessFile(CProcessedFileData &ProcessedData, CFileEntry &Entry);
	
public:
	CFileProcess() 
		: m_u64FilesProcessed(0)
		, m_u64BytesProcessed(0)
	{}
	~CFileProcess() 
	{}

	
	bool ProcessFileList(CProcessedFileData &ProcessedData);

	uint64_t GetTotalFilesProcessed() const
	{
		return m_u64FilesProcessed;
	}
	
	uint64_t GetTotalBytesProcessed() const
	{
		return m_u64BytesProcessed;
	}

	uint64_t GetSecondsProcessed() const
	{
		uint64_t u64Diff = (m_ProcEnd - m_ProcStart);
		return (u64Diff / 10000000);
	}
	
	uint64_t GetFilesPerSecond() const
	{
		uint64_t u64Diff = GetSecondsProcessed();
		if (u64Diff > 0)
		{
			return (m_u64FilesProcessed / u64Diff);
		}
		return m_u64FilesProcessed;
	}

	uint64_t GetBytesPerSecond() const
	{
		uint64_t u64Diff = GetSecondsProcessed();
		if (u64Diff > 0)
		{
			return (m_u64BytesProcessed / u64Diff);
		}
		return m_u64BytesProcessed;
	}
	
};

