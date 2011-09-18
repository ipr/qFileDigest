#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

#include "DigestList.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sNewPath(QString szNewPath);
    void sNewFiles(QStringList lstNewFiles);
    //void sFileProcessed(CFileEntry *pEntry);
    //void sPathsProcessed();
	
private:
    Ui::MainWindow *ui;
	
	// when adding path, 
	// browse from previous selection
	QString m_szPreviousPath;
	
	// GUI-related helpers
	DigestList m_DigestList;
	
private slots:
    void on_actionExpand_Duplicates_triggered();
    void on_actionClear_List_triggered();
    void on_actionCollapse_All_triggered();
    void on_actionExpand_All_triggered();
    void on_actionAbout_triggered();
    void on_actionFile_triggered();
    void on_actionPath_triggered();
	
    void onPathSelected(QString szNewPath);
    void onFilesSelected(QStringList lstNewFiles);
    //void onFileProcessed(CFileEntry *pEntry);
	//void onPathProcessed(long lIndex);
	//void onPathsProcessed();
    
    void onPathMessage(QString szMessage);
	
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
};

#endif // MAINWINDOW_H
