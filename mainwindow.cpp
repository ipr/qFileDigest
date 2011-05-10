#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "detailsdialog.h"

#include <QFileDialog>
#include <QTextEdit>

#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_szPreviousPath(),
	m_DigestList()
{
    ui->setupUi(this);

	connect(this, SIGNAL(sNewPath(QString)), this, SLOT(onPathSelected(QString)));
	connect(this, SIGNAL(sNewFiles(QStringList)), this, SLOT(onFilesSelected(QStringList)));
	//connect(this, SIGNAL(sFileProcessed(CFileEntry*)), this, SLOT(onFileProcessed(CFileEntry*)));
	//connect(this, SIGNAL(sPathProcessed(long)), this, SLOT(onPathProcessed(long)));
	//connect(this, SIGNAL(sPathsProcessed()), this, SLOT(onPathsProcessed()));

	connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
	
	m_DigestList.SetTreeWidget(ui->treeWidget);
	m_DigestList.InitTreeWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionPath_triggered()
{
    // open browse dialog
    QString szPathname = QFileDialog::getExistingDirectory(this, tr("Select folder"), m_szPreviousPath);
    if (szPathname != NULL)
    {
		emit sNewPath(szPathname);
    }
}

void MainWindow::on_actionFile_triggered()
{
	// TODO: multi-file selection?
	/*
    QString szFileName = QFileDialog::getOpenFileName(this, tr("Open file"));
    if (szFileName != NULL)
    {
		QStringList lst;
		lst << szFileName;
		emit sNewFiles(lst);
    }
	*/

	// something like this?
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	if (dialog.exec())
	{
		QStringList fileNames = dialog.selectedFiles();
		if (fileNames.size() > 0)
		{
			emit sNewFiles(fileNames);
		}
	}
}

void MainWindow::on_actionAbout_triggered()
{
	QTextEdit *pTxt = new QTextEdit(this);
	pTxt->setWindowFlags(Qt::Window); //or Qt::Tool, Qt::Dialog if you like
	pTxt->setReadOnly(true);
	pTxt->append("qFileDigest by Ilkka Prusi 2011");
	pTxt->append("");
	pTxt->append("This program is free to use and distribute. No warranties of any kind.");
	pTxt->append("This program uses Qt 4.7.1 under LGPL v. 2.1");
	pTxt->append("This program uses code derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm");
	pTxt->append("");
	/*
	pTxt->append("Keyboard shortcuts:");
	pTxt->append("");
	//pTxt->append("F = open file");
	pTxt->append("Esc = close");
	pTxt->append("");
	*/
	pTxt->show();
    
}

////////

// this could be in another thread? (from GUI-thread)
//
void MainWindow::onPathSelected(QString szNewPath)
{
	ui->statusBar->showMessage("Listing files..");
	
	// locate files recursively
	if (m_DigestList.ListFromPath(szNewPath) == false)
	{
		QMessageBox::warning(this, 
							 tr("Failed listing files"), 
							 tr("Failure listing files"), 
							 QMessageBox::Ok);
		return;
	}
	
	// keep path for next selection
	m_szPreviousPath = szNewPath;
	

	ui->statusBar->showMessage("Processing files..");
	
	// get MD5 and SHA1 sum of each listed file
	if (m_DigestList.ProcessFileList() == false)
	{
	}

	// show some statistics on status-bar
	m_DigestList.ShowStatistics(ui->statusBar);

	m_DigestList.ShowProcessed();
	
	//emit sPathsProcessed();
}

void MainWindow::onFilesSelected(QStringList lstNewFiles)
{
	if (m_DigestList.ListOfFiles(lstNewFiles) == false)
	{
		return;
	}
	
	// get MD5 and SHA1 sum of each listed file
	if (m_DigestList.ProcessFileList() == false)
	{
	}
}

/*
void MainWindow::onFileProcessed(CFileEntry *pEntry)
{
}
*/

/*
void MainWindow::onPathProcessed(long lIndex)
{
}
*/

/*
void MainWindow::onPathsProcessed()
{
	//ui->statusBar->showMessage("Paths processed, displaying files..");

	m_DigestList.ShowProcessed();
	
	//ui->statusBar->clearMessage();
}
*/

void MainWindow::on_actionExpand_All_triggered()
{
	ui->treeWidget->expandAll();
}

void MainWindow::on_actionCollapse_All_triggered()
{
	ui->treeWidget->collapseAll();
}

void MainWindow::on_actionExpand_Duplicates_triggered()
{
	m_DigestList.ExpandToDuplicates();
}

void MainWindow::on_actionClear_List_triggered()
{
	m_DigestList.ClearList();
}

void MainWindow::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	// lookup item-related entry
	// 
	CFileEntry *pEntry = m_DigestList.GetEntryOfItem(item);
	if (pEntry == nullptr)
	{
		return;
	}
	
	/*
	// if archive, extract file list to new dialog and display duplicates
	if (pEntry->m_FileType.IsArchive() == true)
	{
		ArchiveDialog *pArcDlg = new ArchiveDialog(this);
		pArcDlg->SetArchiveEntry(pEntry);
		pArcDlg->show();
		return;
	}
	*/

	/*
	// only one in archive-types currently possibly to show here..
	if (pEntry->m_FileType.m_enFileType == HEADERTYPE_LZX)
	{
		ArchiveDialog *pDlg = new ArchiveDialog(this);
		pDlg->SetParentDigestList(&m_DigestList);
		pDlg->SetArchiveEntry(pEntry);
		pDlg->show();
	}
	*/
	
	// otherwise, just display some data of the file
	DetailsDialog *pDlg = new DetailsDialog(this);
	pDlg->SetParentDigestList(&m_DigestList);
	pDlg->SetFileEntry(pEntry);
	pDlg->show();
	

/*	
	QTextEdit *pTxt = new QTextEdit(this);
	pTxt->setWindowFlags(Qt::Window); //or Qt::Tool, Qt::Dialog if you like
	pTxt->setReadOnly(true);
	
	pTxt->append(QString().append("File: ").append(QString::fromStdWString(pEntry->m_szName)));
	pTxt->append(QString().append("Size: ").append(QString::number(pEntry->m_i64FileSize)));
	//pTxt->append(QString().append("Modified: ").append(QString::number(pEntry->m_LastWrittenTo)));
	pTxt->append(QString().append("Type: ").append(QString::fromStdWString(pEntry->m_FileType.GetNameOfType())));
	pTxt->append(QString().append("Category: ").append(QString::fromStdWString(pEntry->m_FileType.GetNameOfCategory())));
	pTxt->append(QString().append("Header: ").append(QString::fromStdWString(pEntry->m_szHeaderDump)));
	
	pTxt->show();
*/
}
