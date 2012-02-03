///////////////////////////////////////////////
//
// (see notes in header file)
//
// Ilkka Prusi, 2011
//

#include "DigestList.h"

#include <QStatusBar>


QString DigestList::SizeToString(CFileEntry *pEntry) const
{
	QString szSize;
	if (pEntry->m_i64FileSize < 1024)
	{
		szSize.sprintf("%d B", pEntry->m_i64FileSize);
	}
	else if (pEntry->m_i64FileSize >= 1024 && pEntry->m_i64FileSize < 1024*1024)
	{
		szSize.sprintf("%d kB", pEntry->m_i64FileSize/1024);
	}
	else if (pEntry->m_i64FileSize >= 1024*1024 && pEntry->m_i64FileSize < 1024*1024*1024)
	{
		szSize.sprintf("%d MB", pEntry->m_i64FileSize/(1024*1024));
	}
	else
	{
		szSize.sprintf("%d GB", pEntry->m_i64FileSize/(1024*1024*1024));
	}
	return szSize;
}

QString DigestList::GetFullPath(CFileEntry *pEntry) const
{
	QString szPathName = QString::fromStdWString(m_FileData.m_Paths.GetPathByIndex(pEntry->m_lPathIndex));
	if (szPathName.at(szPathName.length()-1) != '\\')
	{
		szPathName += "\\";
	}
	szPathName += QString::fromStdWString(pEntry->m_szName);
	return szPathName;
}

void DigestList::DescribePathToItem(const long lPathIndex, QTreeWidgetItem *pItem) const
{
	// get referenced path from list,
	// set as name to top-level item
	pItem->setText(0, QString::fromStdWString(m_FileData.m_Paths.GetPathByIndex(lPathIndex)));
}

void DigestList::DescribeEntryToItem(CFileEntry *pEntry, QTreeWidgetItem *pItem, bool bIsDuplicate) const
{
	if (bIsDuplicate == true)
	{
		// full pathname
		pItem->setText(0, GetFullPath(pEntry));
	}
	else
	{
		// actual file name
		pItem->setText(0, QString::fromStdWString(pEntry->m_szName));
	}
		
	// get displayable size (shortened)
	pItem->setText(1, SizeToString(pEntry));

	// type
	pItem->setText(2, QString::fromStdWString(pEntry->m_FileType.GetNameOfType()));
	
	// MD5 digest
	pItem->setText(3, QString::fromStdWString(pEntry->m_szMd5));
	
	// SHA-1 digest
	pItem->setText(4, QString::fromStdWString(pEntry->m_szSha1).toLower());
}

QTreeWidgetItem *DigestList::PathToItem(const long lPathIndex)
{
	// note: we need some kind of map
	// since treewidget does not allow "skipping"
	// of index (when no files in a folder) 
	// -> use simple mapping from our path-index 
	// so we only need to show those with actual data/files in them..
	//
	QTreeWidgetItem *pTopItem = m_PathIxToItem.value(lPathIndex, NULL);
	if (pTopItem == NULL)
	{
		// does not exist yet -> add new
		pTopItem = new QTreeWidgetItem((QTreeWidget*)0);
		
		DescribePathToItem(lPathIndex, pTopItem);
		
		m_pWidget->addTopLevelItem(pTopItem);
		
		// map path-index to top-level item in widget
		m_PathIxToItem.insert(lPathIndex, pTopItem);
	}
	return pTopItem;
}

QTreeWidgetItem *DigestList::FileToSubItem(QTreeWidgetItem *pTopItem, CFileEntry *pEntry)
{
	// add file to path-data
	QTreeWidgetItem *pSubItem = m_EntryToItem.value(pEntry, NULL);
	if (pSubItem == NULL)
	{
		// new file-entry item
		pSubItem = new QTreeWidgetItem(pTopItem);
		m_EntryToItem.insert(pEntry, pSubItem); // keep for later

		// helper to change info into displayable format
		DescribeEntryToItem(pEntry, pSubItem);
		
		// add file-entry as child of top-level item
		pTopItem->addChild(pSubItem);
	}
	return pSubItem;
}

QTreeWidgetItem *DigestList::DuplicateFileToItem(QTreeWidgetItem *pSubItem, CFileEntry *pMatch)
{
	QTreeWidgetItem *pDuplicate = new QTreeWidgetItem(pSubItem);
	
	// helper to change info into displayable format
	DescribeEntryToItem(pMatch, pDuplicate, true);
	
	// set duplicate as child of entry
	pSubItem->addChild(pDuplicate);
	
	return pDuplicate;
}

void DigestList::DuplicateFilesToItems(QTreeWidgetItem *pSubItem, CHashMatchList::tMatchList &MatchList)
{
	CHashMatchList::tMatchList::const_iterator itMatch = MatchList.begin();
	CHashMatchList::tMatchList::const_iterator itMatchEnd = MatchList.end();
	while (itMatch != itMatchEnd)
	{
		CFileEntry *pMatch = (*itMatch);
		
		// when adding more paths,
		// check if this entry was already added as duplicate of item
		// so we don't list each duplicate more than once per item-entry..
		//
		if (IsEntryListedAsDuplicate(pSubItem, pMatch) == false)
		{
			// add match as duplicate of listed file-entry
			QTreeWidgetItem *pDuplicate = DuplicateFileToItem(pSubItem, pMatch);
			
			// keep for reverse-mapping item to entry
			m_ItemToEntry.insert(pDuplicate, pMatch);
		}
		
		++itMatch;
	}
}

bool DigestList::IsFileEntryListed(QTreeWidgetItem *pTopItem, CFileEntry *pEntry)
{
	if (pTopItem->childCount() <= 0)
	{
		// no files in folder -> does not yet exist in list
		return false;
	}

	int iCount = pTopItem->childCount();
	for (int i = 0; i < iCount; i++)
	{
		// check if any of duplicate-file entry of the item
		// matches given entry
		QTreeWidgetItem *pSubItem = pTopItem->child(i);
		if (pSubItem->text(0) == QString::fromStdWString(pEntry->m_szName))
		{
			// full path matches 
			// -> already listed as duplicate entry of same file
			return true;
		}
	}

	return false;
}

// is duplicate-entry already listed under item
// when adding paths and finding more duplicates.
//
// sub-item lists each entry which is duplicate,
// check that duplicates are not shown more than once per entry
// when adding more files into information.
//
bool DigestList::IsEntryListedAsDuplicate(QTreeWidgetItem *pSubItem, CFileEntry *pMatch)
{
	if (pSubItem->childCount() <= 0)
	{
		// no duplicates -> does not yet exist in list
		return false;
	}
	
	//QString szFullPath = GetFullPath(pMatch);
	
	int iCount = pSubItem->childCount();
	for (int i = 0; i < iCount; i++)
	{
		// check if any of duplicate-file entry of the item
		// matches given entry
		QTreeWidgetItem *pDuplicate = pSubItem->child(i);

		// locate entry related to item
		CFileEntry *pExisting = m_ItemToEntry.value(pDuplicate, nullptr);
		
		//if (pDuplicate->text(0) == szFullPath)
		if (pMatch->IsSameInstance(*pExisting) == true)
		{
			// full path + name matches 
			// -> already listed as duplicate entry of same file
			return true;
		}
	}
	
	return false;
}


/////////// public methods

DigestList::DigestList()
	: m_pWidget(nullptr),
	m_FileData(),
	m_ProcFile(),
    m_Counters(),
	m_PathIxToItem(),
	m_EntryToItem(),
	m_ItemToEntry()
{
}

DigestList::~DigestList()
{
	// just clear, actual tree-widget should destroy items
	m_PathIxToItem.clear();
	m_EntryToItem.clear();
	m_ItemToEntry.clear();
}

void DigestList::SetTreeWidget(QTreeWidget *pView)
{
	m_pWidget = pView;
}

void DigestList::InitTreeWidget()
{
	QStringList treeHeaders;
	//treeHeaders << "Path" << "File" << "Size" << "Checksum";
	//treeHeaders << "Name" << "Size" << "MD5" << "SHA-1";

	treeHeaders << "Name" << "Size" << "Type" << "MD5" << "SHA-1";
	
	m_pWidget->setColumnCount(treeHeaders.size());
	m_pWidget->setHeaderLabels(treeHeaders);
}

void DigestList::ClearList()
{
	// just clear, actual tree-widget should destroy items
	m_PathIxToItem.clear();
	m_EntryToItem.clear();
	m_ItemToEntry.clear();
	
    m_pWidget->clear();
}

bool DigestList::ListFromPath(QString &szNewPath)
{
    // process given path
    wstring szPath = szNewPath.toStdWString();
    CFileListHandler FileLister(szPath); // root path lister

	// locate files recursively
    if (FileLister.ListFiles(m_FileData) == false)
    {
		return false;
	}

    if (m_FileData.GetFileCount() == 0)
    {
		// nothing found in path, no error
		return true;
    }
	
	return true;
}

bool DigestList::ListOfFiles(QStringList &lstFiles)
{
	auto it = lstFiles.begin();
	auto itEnd = lstFiles.end();
	while (it != itEnd)
	{
		if ((*it).length() > 0)
		{
			ListFile((*it));
		}
		++it;
	}
	return true;
}

bool DigestList::ListFile(QString &szFilePath)
{
	// TODO:
	// - parse to path and file name
	// - add path to path lookup list
	// - add file to file entry list
	// - get metadata of file
	// - process it

	if (szFilePath.length() < 1)
	{
		return false;
	}
	
	szFilePath.replace('/', "\\");
	int iPos = szFilePath.lastIndexOf('\\');
	if (iPos == -1)
	{
		// no path
		return false;
	}
	
	QString szFile = szFilePath.right(szFilePath.length() - iPos -1);
	wstring szPathName = szFilePath.left(iPos).toStdWString();
	long lPathIndex = m_FileData.m_Paths.AddPath(szPathName);
	
	// get statistics of file (modification time, size etc.)
	CFileListHandler List(szPathName);
	return List.ListFile(lPathIndex, szFilePath.toStdWString(), m_FileData);
}

bool DigestList::ProcessFileList()
{
	// reset on start
	m_Counters.ClearCounters();
	
    if (m_FileData.GetFileCount() == 0)
    {
		// nothing found in path, no error
		return true;
    }

    CProcessedFileData::tFileList::iterator itList = m_FileData.m_vFileList.begin();
    CProcessedFileData::tFileList::iterator itListEnd = m_FileData.m_vFileList.end();
    while (itList != itListEnd)
    {
        CFileEntry *pEntry = (*itList);

        // get MD5 and SHA1 sum of each file
        m_ProcFile.ProcessFile(m_FileData, (*pEntry));

        /*
        if (pEntry->m_FileType.IsArchive() == true)
        {
            // unpack/uncompress/decrunch archive
            // and check individual files also?

        }
        */
        
		// update counters on each file
		m_Counters.UpdateOnEntry(pEntry);

        ++itList;
    }

    // update time of ending now
    m_Counters.m_ProcEnd.SetNow();

    return true;
}

void DigestList::ShowProcessed()
{
	// lookup hash-match for each file in list,
	// show duplicates on each file (if any)
	//
	CProcessedFileData::tFileList::const_iterator it = m_FileData.m_vFileList.begin();
	CProcessedFileData::tFileList::const_iterator itEnd = m_FileData.m_vFileList.end();
	while (it != itEnd)
	{
		CFileEntry *pEntry = (*it);

		// check that file and path are in widget-display before adding duplicates to list
		QTreeWidgetItem *pTopItem = PathToItem(pEntry->m_lPathIndex);
		QTreeWidgetItem *pSubItem = FileToSubItem(pTopItem, pEntry);
		
		// keep for reverse-mapping item to entry
		m_ItemToEntry.insert(pSubItem, pEntry);
		
		// list each found duplicate under the entry
		// without itself for simple lookup in GUI
		//
		DuplicateFilesToItems(pSubItem, m_FileData.GetMatchListOfEntry(pEntry));
		
		++it;
	}
	m_pWidget->resizeColumnToContents(0);
}

void DigestList::ExpandToDuplicates()
{
	int iTopCount = m_pWidget->topLevelItemCount();
	for (int i = 0; i < iTopCount; i++)
	{
		QTreeWidgetItem *pTopItem = m_pWidget->topLevelItem(i);
		int iSubCount = pTopItem->childCount();
		for (int j = 0; j < iSubCount; j++)
		{
			QTreeWidgetItem *pSubItem = pTopItem->child(j);
			if (pSubItem->childCount() > 0)
			{
				pTopItem->setExpanded(true);
				pSubItem->setExpanded(true);
			}
		}
	}
}

void DigestList::ShowStatistics(QStatusBar *pStatusBar)
{
	if (pStatusBar == NULL
		|| pStatusBar == nullptr)
	{
		return;
	}
	
	// count some statistics
	uint64_t u64Seconds = m_Counters.GetSecondsProcessed();
	uint64_t u64FilesPerSec = m_Counters.GetFilesPerSecond();
	uint64_t u64BytesPerSec = m_Counters.GetBytesPerSecond();

	QString szStat = QString("Files/s: %1, Bytes/s: %2, Time used (s): %3, File count: %4, Byte count: %5")
					 .arg(u64FilesPerSec)
					 .arg(u64BytesPerSec)
					 .arg(u64Seconds)
					 .arg(m_Counters.GetTotalFilesProcessed())
					 .arg(m_Counters.GetTotalBytesProcessed());
	pStatusBar->showMessage(szStat);
}

CFileEntry *DigestList::GetEntryOfItem(QTreeWidgetItem *pItem)
{
	return m_ItemToEntry.value(pItem, nullptr);

	/*	
	auto it = m_EntryToItem.begin();
	auto itEnd = m_EntryToItem.end();
	while (it != itEnd)
	{
		if (it.value() == pItem)
		{
			return it.key();
		}
		++it;
	}
	return nullptr;
	*/
}

QString DigestList::GetFullPathToEntry(CFileEntry *pEntry)
{
	// note: this may need locking..
	return GetFullPath(pEntry);
}

QString DigestList::GetPathToEntry(CFileEntry *pEntry)
{
	return QString::fromStdWString(m_FileData.m_Paths.GetPathByIndex(pEntry->m_lPathIndex));
}

