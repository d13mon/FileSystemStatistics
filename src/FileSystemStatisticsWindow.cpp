#include "FileSystemStatisticsWindow.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QDebug>
#include <QMessageBox>

FileSystemStatisticsWindow::FileSystemStatisticsWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this); 
	
	mFilesystemModel = new QFileSystemModel;
	mFilesystemModel->setRootPath(QDir::rootPath());
	ui.filesystemTree->setModel(mFilesystemModel);

	mStatisticsProvider = new StatisticsProvider(this);

	initWidgets();

	updateStatus(OperationStatus::WAITING);

	connect(ui.filesystemTree, &QTreeView::activated, [this](const QModelIndex& index) {		
		auto fi = mFilesystemModel->fileInfo(index);
		if (fi.isDir()) {
			updateStatus(OperationStatus::PROCESSING);
			ui.statusSelectedDirEdit->setText(fi.absoluteFilePath());
			mStatisticsProvider->start(fi);
		}
		else
		{
			QMessageBox::information(this, windowTitle(), tr("Please select a directory."), QMessageBox::Ok);
		}		
	});
}

FileSystemStatisticsWindow::~FileSystemStatisticsWindow()
{
	delete mStatisticsProvider;
}

void FileSystemStatisticsWindow::initWidgets()
{	
	ui.filesystemTree->setColumnWidth(0, 300);
	ui.filesystemTree->setColumnHidden(1, true);
	ui.filesystemTree->setColumnHidden(2, true);
	ui.filesystemTree->setColumnHidden(3, true);
	ui.filesystemTree->setHeaderHidden(true);
	
	menuBar()->hide();
	ui.mainToolBar->hide();
	statusBar()->addWidget(ui.statusFrame, 1);
	ui.statusOperationEdit->setFixedWidth(250);

	QSplitter * splitterMain{ new QSplitter(Qt::Horizontal, this) };
	splitterMain->setContentsMargins(5, 5, 5, 5);
	splitterMain->addWidget(ui.filesystemTree);
	splitterMain->addWidget(ui.statisticsGroup);
	splitterMain->setStretchFactor(0, 1);
	splitterMain->setStretchFactor(1, 4);

	setCentralWidget(splitterMain);
}

void FileSystemStatisticsWindow::updateStatus(OperationStatus status)
{
	mStatus = status;
	ui.statusOperationEdit->setText(toString(mStatus));
}

void FileSystemStatisticsWindow::closeEvent(QCloseEvent* event)
{
	mStatisticsProvider->stop();

	QMainWindow::closeEvent(event);
}
