#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>

#include "FileType.h"
#include "FileListHandler.h"
#include "DigestList.h"


namespace Ui {
    class DetailsDialog;
}

class DetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetailsDialog(QWidget *parent = 0);
    ~DetailsDialog();
	
	void SetParentDigestList(DigestList *pList);
	void SetFileEntry(CFileEntry *pFileEntry);

private:
    Ui::DetailsDialog *ui;
	
	// GUI-related helpers
	DigestList *m_pParentDigestList; // parent
	CFileEntry *m_pCurrentEntry;

	// selected archive-file contents
	DigestList m_DigestList;

	void ShowEntryDetails();
	void ShowArchiveList();
	
private slots:
    void on_cmdDecrunch_clicked();
};

#endif // DETAILSDIALOG_H
