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
	
	void SetDigestList(DigestList *pDigestList);
	void SetFileEntry(CFileEntry *pFileEntry);

private:
    Ui::DetailsDialog *ui;
	DigestList *m_pDigestList;
	CFileEntry *m_pCurrentEntry;

	void ShowEntryDetails();
	
private slots:
    void on_cmdDecrunch_clicked();
};

#endif // DETAILSDIALOG_H
