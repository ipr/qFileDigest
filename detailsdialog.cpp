#include "detailsdialog.h"
#include "ui_detailsdialog.h"

#include "PowerPacker.h"

#include <QDateTime>


DetailsDialog::DetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailsDialog),
	m_pDigestList(nullptr),
	m_pCurrentEntry(nullptr)
{
    ui->setupUi(this);
	ui->lblDecrunch->setHidden(true);
	ui->cmdDecrunch->setHidden(true);
}

DetailsDialog::~DetailsDialog()
{
    delete ui;
}

void DetailsDialog::SetDigestList(DigestList *pDigestList)
{
	m_pDigestList = pDigestList;
	if (m_pDigestList == nullptr
		|| m_pCurrentEntry == nullptr)
	{
		return;
	}
	
	ShowEntryDetails();	
}

void DetailsDialog::SetFileEntry(CFileEntry *pFileEntry)
{
	m_pCurrentEntry = pFileEntry;
	if (m_pDigestList == nullptr
		|| m_pCurrentEntry == nullptr)
	{
		return;
	}

	ShowEntryDetails();	
}

void DetailsDialog::ShowEntryDetails()
{
	QString szFile = m_pDigestList->GetFullPathToEntry(m_pCurrentEntry);
	setWindowTitle(szFile);
	
	ui->txtPath->setText(m_pDigestList->GetPathToEntry(m_pCurrentEntry));
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
	
	switch (m_pCurrentEntry->m_FileType.m_enFileType)
	{
	case HEADERTYPE_PP20:
		bIsDecrunchAvailable = true;
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
}

void DetailsDialog::on_cmdDecrunch_clicked()
{
	QString szFile = m_pDigestList->GetFullPathToEntry(m_pCurrentEntry);
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
			//m_pDigestList->ShowProcessed();
			ui->cmdDecrunch->setHidden(true);
		}
		catch (PPException &exp)
		{
			ui->lblDecrunch->setText(QString::fromLocal8Bit(exp.what()));
		}
	}
}
