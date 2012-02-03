#include "detailsdialog.h"
#include "ui_detailsdialog.h"

#include "PowerPacker.h"
#include "ImploderExploder.h"
#include "qlhalib.h"
#include "qlzxlib.h"

#include <QDateTime>
#include <QMessageBox>


DetailsDialog::DetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailsDialog),
	m_pLhaLib(nullptr),
	m_pLzxLib(nullptr),
	m_pParentDigestList(nullptr),
	m_pCurrentEntry(nullptr)
{
    ui->setupUi(this);
	
	// don't show unless supported
	ui->lblDecrunch->setHidden(true);
	ui->cmdDecrunch->setHidden(true);
	ui->archiveList->setHidden(true);

	m_DigestList.SetTreeWidget(ui->archiveList);
	m_DigestList.InitTreeWidget();
}

DetailsDialog::~DetailsDialog()
{
    delete ui;
}

void DetailsDialog::SetParentDigestList(DigestList *pList)
{
	m_pParentDigestList = pList;
	if (m_pParentDigestList == nullptr
		|| m_pCurrentEntry == nullptr)
	{
		return;
	}
	
	ShowEntryDetails();	
}

void DetailsDialog::SetFileEntry(CFileEntry *pFileEntry)
{
	m_pCurrentEntry = pFileEntry;
	if (m_pParentDigestList == nullptr
		|| m_pCurrentEntry == nullptr)
	{
		return;
	}

	ShowEntryDetails();	
}

void DetailsDialog::ShowEntryDetails()
{
	QString szFile = m_pParentDigestList->GetFullPathToEntry(m_pCurrentEntry);
	setWindowTitle(szFile);
	
	ui->txtPath->setText(m_pParentDigestList->GetPathToEntry(m_pCurrentEntry));
	ui->txtFileName->setText(QString::fromStdWString(m_pCurrentEntry->m_szName));
	ui->txtFileSize->setText(QString::number(m_pCurrentEntry->m_i64FileSize));
	ui->txtFileType->setText(QString::fromStdWString(m_pCurrentEntry->m_FileType.GetNameOfType()));
	ui->txtCategory->setText(QString::fromStdWString(m_pCurrentEntry->m_FileType.GetNameOfCategory()));
	
	ui->txtMd5->setText(QString::fromStdWString(m_pCurrentEntry->m_szMd5));
	ui->txtSha1->setText(QString::fromStdWString(m_pCurrentEntry->m_szSha1).toLower());
	
	ui->txtHeader->setText(QString::fromStdWString(m_pCurrentEntry->m_szHeaderDump));
	ui->txtAscii->setText(QString::fromStdWString(m_pCurrentEntry->m_szHeaderAscii));
	
	QDateTime Stamp = QDateTime::fromTime_t(m_pCurrentEntry->m_LastWrittenTo.GetAsUnixTime());
	//ui->txtModificationStamp->setText(Stamp.toString(Qt::SystemLocaleLongDate));
	ui->txtModificationStamp->setText(Stamp.toString(Qt::ISODate));
	
	bool bIsDecrunchAvailable = false;
	bool bIsArchiveViewAvailable = false;
	
	// TODO: get file type info with shared library:
	//qXpkLib->setInputFile();
	//qXpkLib->xpkInfo();
	
	switch (m_pCurrentEntry->m_FileType.m_enFileType)
	{
	case HEADERTYPE_PP20:
	case HEADERTYPE_IMPLODER:
		bIsDecrunchAvailable = true;
		break;
		
	case HEADERTYPE_LHA:
	case HEADERTYPE_LZX:
		bIsArchiveViewAvailable = true;
		break;
		
	case HEADERTYPE_XPK_GENERIC:
	case HEADERTYPE_XPK_SQSH:
	case HEADERTYPE_XPK_NUKE:
		//bIsDecrunchAvailable = true;
		break;
	}
	
	if (bIsDecrunchAvailable == true)
	{
		ui->lblDecrunch->setHidden(false);
		ui->cmdDecrunch->setHidden(false);
	}
	if (bIsArchiveViewAvailable == true)
	{
		ui->archiveList->setHidden(false);
		ShowArchiveList();
	}
}

void DetailsDialog::ShowArchiveList()
{
	// show list of files in given archive-file
	
	QString szFile = m_pParentDigestList->GetFullPathToEntry(m_pCurrentEntry);
	setWindowTitle(szFile);
	
	if (m_pCurrentEntry->m_FileType.m_enFileType == HEADERTYPE_LZX)
	{
		if (m_pLzxLib == nullptr)
		{
			m_pLzxLib = new QLZXLib(this);
		}
		
		try
		{
			QLZXLib::tEntryInfoList lstArchiveInfo;
			
			// set given file as archive
			m_pLzxLib->SetArchive(szFile);
		
			// collect list of files
			m_pLzxLib->List(lstArchiveInfo);
			
			// note: following is temp listing..
			
			QTreeWidgetItem *pTopItem = new QTreeWidgetItem((QTreeWidgetItem*)0);
			pTopItem->setText(0, szFile);
			ui->archiveList->addTopLevelItem(pTopItem);
			
			auto it = lstArchiveInfo.begin();
			auto itEnd = lstArchiveInfo.end();
			while (it != itEnd)
			{
				QLZXLib::CEntryInfo &Entry = (*it);
				if (Entry.m_szFileName.length() < 1)
				{
					++it;
					continue;
				}
				
				QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pTopItem);
				pSubItem->setText(0, Entry.m_szFileName);
				pSubItem->setText(1, QString::number(Entry.m_ulUnpackedSize)); // always given
				
				// TODO: need to extract files to get hashes..
				//
				// also, add each extracted file to our list
				// so we can lookup hash-matches:
				// set current archive as parent of each of those
				//
				//CFileEntry *pEntry = new CFileEntry();
				//pEntry->m_pParentEntry = m_pCurrentEntry;
				//m_DigestList.m_FileData.AddFoundFile();
				
				pTopItem->addChild(pSubItem);
				++it;
			}
			
			ui->archiveList->resizeColumnToContents(0);
		}
		catch (std::exception &exp)
		{
			QMessageBox::warning(this, "Error viewing",
								 QString::fromLocal8Bit(exp.what()),
								 QMessageBox::Ok);
								 
			//ui->lblDecrunch->setText(QString::fromLocal8Bit(exp.what()));
		}
	}
	else if (m_pCurrentEntry->m_FileType.m_enFileType == HEADERTYPE_LHA)
	{
		if (m_pLhaLib == nullptr)
		{
			m_pLhaLib = new QLhALib(this);
		}
		
		try
		{
			QLhALib::tArchiveEntryList lstArchiveInfo;
			
			// set given file as archive
			m_pLhaLib->SetArchive(szFile);
		
			// collect list of files
			m_pLhaLib->List(lstArchiveInfo);

			// note: following is temp listing..
			
			QTreeWidgetItem *pTopItem = new QTreeWidgetItem((QTreeWidgetItem*)0);
			pTopItem->setText(0, szFile);
			ui->archiveList->addTopLevelItem(pTopItem);
			
			auto it = lstArchiveInfo.begin();
			auto itEnd = lstArchiveInfo.end();
			while (it != itEnd)
			{
				QLhALib::CArchiveEntry &Entry = (*it);
				if (Entry.m_szFileName.length() < 1)
				{
					++it;
					continue;
				}
				
				QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pTopItem);
				pSubItem->setText(0, Entry.m_szFileName);
				pSubItem->setText(1, QString::number(Entry.m_ulUnpackedSize)); // always given
				
				// TODO: need to extract files to get hashes..
				//
				// also, add each extracted file to our list
				// so we can lookup hash-matches:
				// set current archive as parent of each of those
				//
				//CFileEntry *pEntry = new CFileEntry();
				//pEntry->m_pParentEntry = m_pCurrentEntry;
				//m_DigestList.m_FileData.AddFoundFile();
				
				pTopItem->addChild(pSubItem);
				++it;
			}
			
			ui->archiveList->resizeColumnToContents(0);
		}
		catch (std::exception &exp)
		{
			QMessageBox::warning(this, "Error viewing",
								 QString::fromLocal8Bit(exp.what()),
								 QMessageBox::Ok);
								 
			//ui->lblDecrunch->setText(QString::fromLocal8Bit(exp.what()));
		}
	}
}

void DetailsDialog::on_cmdDecrunch_clicked()
{
	QString szFile = m_pParentDigestList->GetFullPathToEntry(m_pCurrentEntry);
	if (m_pCurrentEntry->m_FileType.m_enFileType == HEADERTYPE_PP20)
	{
		try
		{
			CPowerPacker PP;
			PP.UnpackFile(szFile.toLocal8Bit().constData());
			PP.SaveToFile(szFile.toLocal8Bit().constData());
			
			ui->lblDecrunch->setText("Decrunch done");
			
			//m_pDigestList->ListFile(szFile);
			
			// get MD5 and SHA1 hashes etc.
			//m_pDigestList->ProcessFileList();
			ui->cmdDecrunch->setHidden(true);
		}
		catch (std::exception &exp) // catch by base-type
		{
			ui->lblDecrunch->setText(QString::fromLocal8Bit(exp.what()));
		}
	}
	else if (m_pCurrentEntry->m_FileType.m_enFileType == HEADERTYPE_IMPLODER)
	{
		try
		{
			CImploderExploder Impl;
			Impl.UnpackFile(szFile.toLocal8Bit().constData());
			Impl.SaveToFile(szFile.toLocal8Bit().constData());
			
			ui->lblDecrunch->setText("Decrunch done");
			
			//m_pDigestList->ListFile(szFile);
			
			// get MD5 and SHA1 hashes etc.
			//m_pDigestList->ProcessFileList();
			ui->cmdDecrunch->setHidden(true);
		}
		catch (std::exception &exp) // catch by base-type
		{
			ui->lblDecrunch->setText(QString::fromLocal8Bit(exp.what()));
		}
	}
}
