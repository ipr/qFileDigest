/////////////////////////////////////////////////////////////////////
//
// GUI-related stuff in managing how to show file-related info.
//
// Ilkka Prusi, 2011
//


#ifndef DIGESTLIST_H
#define DIGESTLIST_H


#include "FileListHandler.h"

#include <QString>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPair>
#include <QMap>

class QStatusBar;


// TODO: inherit from model instead?


class DigestList
{
private:
	// pointer to widget in UI
	QTreeWidget *m_pWidget;

	
	// actual file&path processing and related data
	CProcessedFileData m_FileData;

	// file processing and statistic-counting
    CFileProcess m_ProcFile;

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

