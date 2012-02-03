/////////////////////////////////////////////////////////////////////
//
// GUI-related stuff in managing how to show file-related info.
//
// Ilkka Prusi, 2011
//


#ifndef DIGESTLIST_H
#define DIGESTLIST_H

// standard typedefs
#include <stdint.h>

#include "FileListHandler.h"
#include "FiletimeHelper.h"

#include <QString>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPair>
#include <QMap>


class QStatusBar;

class DigestCounters
{
public:
	// internal counters for statistics of processed files

	// count of files processed
	uint64_t m_u64FilesProcessed;
	
	// TODO: amount of bytes in each file..
	// can wraparound , how should we count?
	uint64_t m_u64BytesProcessed;

	// entire processing time counting
	CFiletimeHelper m_ProcStart;
	CFiletimeHelper m_ProcEnd;
	
	// per-file time counting
	//FILETIME m_ftFileStart;
	//FILETIME m_ftFileEnd;

public:
	DigestCounters() 
		: m_u64FilesProcessed(0)
		, m_u64BytesProcessed(0)
	{}
	~DigestCounters() {}
	
	void ClearCounters()
	{
		m_u64FilesProcessed = 0;
		m_u64BytesProcessed = 0;
	
		// reset on start
		m_ProcStart.SetNow();
		m_ProcEnd = m_ProcStart;
	}
	
	void UpdateOnEntry(CFileEntry *pEntry)
	{
		++m_u64FilesProcessed;
		m_u64BytesProcessed += pEntry->m_i64FileSize;
	}
	
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

class DigestList
{
// TODO: inherit from model instead?

private:
	// pointer to widget in UI
	QTreeWidget *m_pWidget;

	
	// actual file&path processing and related data
	CProcessedFileData m_FileData;

	// file processing and statistic-counting
    CFileProcess m_ProcFile;
    
    // statistical counters for display
    DigestCounters m_Counters;

	// map path index to top-level item:
	// all paths are not listed since they might not have any files..
	//
	// note: destroyed by actual tree-widget, just keep pointers here.
	//
	QMap<long, QTreeWidgetItem*> m_PathIxToItem;

	// map file entry to item in list:
	// this way we can locate each file in list
	// so we can also show duplicates afterwards
	//
	// note: destroyed separately, just keep pointers here.
	//
	QMap<CFileEntry*, QTreeWidgetItem*> m_EntryToItem;


	// map item to file entry:
	// for lookup of user-action related file
	//
	// note: destroyed separately, just keep pointers here.
	//
	QMap<QTreeWidgetItem*, CFileEntry*> m_ItemToEntry;
	
protected:

	QString SizeToString(CFileEntry *pEntry) const;
	QString GetFullPath(CFileEntry *pEntry) const;
	void DescribePathToItem(const long lPathIndex, QTreeWidgetItem *pItem) const;
	void DescribeEntryToItem(CFileEntry *pEntry, QTreeWidgetItem *pItem, bool bIsDuplicate = false) const;

	// create new item if necessary,
	// update lookup list and give item to caller
    QTreeWidgetItem *PathToItem(const long lPathIndex);
    QTreeWidgetItem *FileToSubItem(QTreeWidgetItem *pTopItem, CFileEntry *pEntry);

    QTreeWidgetItem *DuplicateFileToItem(QTreeWidgetItem *pSubItem, CFileEntry *pMatch);
    void DuplicateFilesToItems(QTreeWidgetItem *pSubItem, CHashMatchList::tMatchList &MatchList);
	
	// is duplicate-entry already listed under item
	// when adding paths and finding more duplicates
	//
	bool IsFileEntryListed(QTreeWidgetItem *pTopItem, CFileEntry *pEntry);
	bool IsEntryListedAsDuplicate(QTreeWidgetItem *pSubItem, CFileEntry *pMatch);
	
public:
    DigestList();
    ~DigestList();
	
	void SetTreeWidget(QTreeWidget *pView);
	
	void InitTreeWidget();
	void ClearList();

	bool ListFromPath(QString &szNewPath);
	bool ListOfFiles(QStringList &lstFiles);
	bool ListFile(QString &szFilePath);

	bool ProcessFileList();
	
	void ShowProcessed();
	void ExpandToDuplicates();
	
	void ShowStatistics(QStatusBar *pStatusBar);
	
	CFileEntry *GetEntryOfItem(QTreeWidgetItem *pItem);
	
	QString GetFullPathToEntry(CFileEntry *pEntry);
	QString GetPathToEntry(CFileEntry *pEntry);
};

#endif // DIGESTLIST_H

