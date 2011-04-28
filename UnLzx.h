#ifndef _UNLZX_H_
#define _UNLZX_H_

#include <string>
#include <list>
#include <exception>

#include "AnsiFile.h"


// exception-classes for error cases
class IOException : public std::exception
{
public:
	IOException(const char *szMessage)
		: std::exception(szMessage)
	{
	}
};

class ArcException : public std::exception
{
protected:
	std::string m_szArchive;
public:
	ArcException(const char *szMessage, const std::string &szArchive)
		: std::exception(szMessage)
		, m_szArchive(szArchive)
	{
	}
	std::string GetArchive()
	{
		return m_szArchive;
	}
};


struct tLzxInfoHeader
{
	unsigned char info_header[10];

	bool IsLzx()
	{
		if ((info_header[0] == 76) && (info_header[1] == 90) && (info_header[2] == 88)) /* LZX */
		{
			return true;
		}
		return false;
	}
};

//typedef unsigned int uint;

// header of single entry in LZX-archive
struct tLzxArchiveHeader
{
	unsigned char archive_header[31];

	unsigned int TakeCrcBytes()
	{
		unsigned int crc = (archive_header[29] << 24) + (archive_header[28] << 16) + (archive_header[27] << 8) + archive_header[26];

		/* Must set the field to 0 before calculating the crc */
		archive_header[29] = 0;
		archive_header[28] = 0;
		archive_header[27] = 0;
		archive_header[26] = 0;

		return crc;
	}

	unsigned char GetAttributes()
	{
		return archive_header[0]; /* file protection modes */
	}

	unsigned char GetPackMode()
	{
		return archive_header[11]; /* pack mode */
	}

	unsigned int GetPackSize()
	{
		return (archive_header[9] << 24) + (archive_header[8] << 16) + (archive_header[7] << 8) + archive_header[6]; /* packed size */
	}

	unsigned int GetUnpackSize()
	{
		return (archive_header[5] << 24) + (archive_header[4] << 16) + (archive_header[3] << 8) + archive_header[2]; /* unpack size */
	}

	unsigned int GetTimestamp()
	{
		return (archive_header[18] << 24) + (archive_header[19] << 16) + (archive_header[20] << 8) + archive_header[21]; /* date */
	}

	void GetTimestampParts(unsigned int &year, unsigned int &month, unsigned int &day, unsigned int &hour, unsigned int &minute, unsigned int &second)
	{
		unsigned int temp = GetTimestamp();

		// split value into timestamp-parts
		year = ((temp >> 17) & 63) + 1970;
		month = (temp >> 23) & 15;
		day = (temp >> 27) & 31;
		hour = (temp >> 12) & 31;
		minute = (temp >> 6) & 63;
		second = temp & 63;
	}

	unsigned int GetFileNameLength()
	{
		return archive_header[30]; /* filename length */
	}

	unsigned int GetCommentLength()
	{
		return archive_header[14]; /* comment length */
	}
};

// describe single entry within LZX-archive
class CArchiveEntry
{
public:
	// these match Amiga-style file-attributes
	// (protection mode flags):
	// HSPA RWED
	struct tFileAttributes
	{
		tFileAttributes()
		{
			h = false;
			s = false;
			p = false;
			a = false;

			r = false;
			w = false;
			e = false;
			d = false;
		}

		bool h;
		bool s;
		bool p;
		bool a;
		bool r;
		bool w;
		bool e;
		bool d;
	};
	struct tFileAttributes m_Attributes;

public:
	CArchiveEntry(void)
		: m_Attributes()
		, m_Header()
		, m_uiCrc(0)
		, m_ulUnpackedSize(0)
		, m_bPackedSizeAvailable(true)
		, m_ulPackedSize(0)
		, m_bIsMerged(false)
		, m_szFileName()
		, m_szComment()
	{}
	~CArchiveEntry(void)
	{}

	/* file protection modes */
	void ParseAttributes()
	{
		unsigned char attrib = m_Header.GetAttributes();
		m_Attributes.h = ((attrib & 32) ? true : false);
		m_Attributes.s = ((attrib & 64) ? true : false);
		m_Attributes.p = ((attrib & 128) ? true : false);
		m_Attributes.a = ((attrib & 16) ? true : false);
		m_Attributes.r = ((attrib & 1) ? true : false);
		m_Attributes.w = ((attrib & 2) ? true : false);
		m_Attributes.e = ((attrib & 8) ? true : false);
		m_Attributes.d = ((attrib & 4) ? true : false);
	};
	void CheckPackedSize()
	{
		if (m_Header.archive_header[12] & 1)
		{
			m_bPackedSizeAvailable = false;
		}
		if ((m_Header.archive_header[12] & 1) && m_ulPackedSize)
		{
			m_bIsMerged = true;
		}
	}

	tLzxArchiveHeader m_Header;
	unsigned int m_uiCrc;

	unsigned long m_ulUnpackedSize;

	// for some text-files, packed size might not be given in archive
	// (merged only?)
	bool m_bPackedSizeAvailable;
	unsigned long m_ulPackedSize;

	bool m_bIsMerged;

	std::string m_szFileName;
	std::string m_szComment;
};

// list of each entry in single archive
typedef std::list<CArchiveEntry> tArchiveEntryList;

// single archive
class CLzxArchive
{
public:
	CLzxArchive(void)
		: m_InfoHeader()
		, m_EntryList()
		, m_szName()
		, m_uiCrc(0)
		, m_nSize(0)
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulMergeSize(0)
		, m_ulTotalFiles(0)
	{}
	CLzxArchive(const std::string &szName)
		: m_InfoHeader()
		, m_EntryList()
		, m_szName(szName)
		, m_uiCrc(0)
		, m_nSize(0)
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulMergeSize(0)
		, m_ulTotalFiles(0)
	{}
	CLzxArchive(const tLzxInfoHeader &InfoHeader)
		: m_InfoHeader(InfoHeader)
		, m_EntryList()
		, m_szName()
		, m_uiCrc(0)
		, m_nSize(0)
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulMergeSize(0)
		, m_ulTotalFiles(0)
	{}
	~CLzxArchive(void)
	{}

	tLzxInfoHeader m_InfoHeader;
	tArchiveEntryList m_EntryList;

	std::string m_szName;
	unsigned int m_uiCrc;

	size_t m_nSize; // filesize of archive in bytes

	unsigned long m_ulTotalUnpacked;
	unsigned long m_ulTotalPacked;
	unsigned long m_ulMergeSize;

	unsigned long m_ulTotalFiles;
};

// list of archives
typedef std::list<CLzxArchive> tArchiveList;


class CUnLzx
{
protected:
	void ReadInfoHeader(const std::string &szArchive, CAnsiFile &ArchiveFile, tLzxInfoHeader &InfoHeader) const;
	void ReadString(CAnsiFile &ArchiveFile, const unsigned int uiSize, unsigned char *pBuffer) const;

	bool ViewArchive(CLzxArchive &ArchiveInfo);
	bool ExtractArchive(const std::string &szArchive);

public:

	/*
	typedef enum tMode
	{
		MODE_UNKNOWN = 0,
		MODE_VIEW = 1,
		MODE_EXTRACT = 2
	};
	tMode m_enMode;
	*/

	CUnLzx(void);
	~CUnLzx(void);

	// do a single archive
	bool View(CLzxArchive &ArchiveInfo);

	// do multiple archives
	bool ViewList(std::list<std::string> &lstArchives, tArchiveList &ArcList);

	// do a single archive
	bool Extract(std::string &szArchive);

	// do multiple archives
	bool ExtractList(std::list<std::string> &lstArchives);

};


#endif // ifndef _UNLZX_H_

