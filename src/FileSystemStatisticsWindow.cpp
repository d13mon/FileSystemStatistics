#include "FileSystemStatisticsWindow.h"

#include <QSplitter>
#include <QFileSystemModel>

FileSystemStatisticsWindow::FileSystemStatisticsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	initWidgets();
}

void FileSystemStatisticsWindow::initWidgets()
{
	QFileSystemModel *model = new QFileSystemModel;
	model->setRootPath(QDir::rootPath());
	ui.filesystemTree->setModel(model);
	ui.filesystemTree->setColumnWidth(0, 300);
	ui.filesystemTree->setColumnHidden(1, true);
	ui.filesystemTree->setColumnHidden(2, true);
	ui.filesystemTree->setColumnHidden(3, true);
	
	menuBar()->hide();
	ui.mainToolBar->hide();

	QSplitter * splitterMain{ new QSplitter(Qt::Horizontal, this) };
	splitterMain->setContentsMargins(5, 5, 5, 5);
	splitterMain->addWidget(ui.filesystemTree);
	splitterMain->addWidget(ui.statisticsGroup);
	splitterMain->setStretchFactor(0, 1);
	splitterMain->setStretchFactor(1, 4);

	setCentralWidget(splitterMain);
}
