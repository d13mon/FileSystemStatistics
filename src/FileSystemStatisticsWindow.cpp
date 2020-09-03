#include "FileSystemStatisticsWindow.h"
#include "StatisticsTableModel.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

FileSystemStatisticsWindow::FileSystemStatisticsWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this); 
	
	mFilesystemModel = new QFileSystemModel;
	mFilesystemModel->setRootPath(QDir::rootPath());	

	mStatisticsModel = new StatisticsTableModel(this);	

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
	delete mStatisticsModel;
	delete mFilesystemModel;
}

void FileSystemStatisticsWindow::initWidgets()
{	
	ui.filesystemTree->setModel(mFilesystemModel);
	ui.filesystemTree->setColumnWidth(1, 300);	
	ui.filesystemTree->setColumnHidden(1, true);
	ui.filesystemTree->setColumnHidden(2, true);
	ui.filesystemTree->setColumnHidden(3, true);	
	ui.filesystemTree->setHeaderHidden(true);
	

	ui.statisticsTable->setModel(mStatisticsModel);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColFilesSize, 200);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColAvgFileSize, 200);
//	ui.statisticsTable->horizontalHeader()->setGr
	
	menuBar()->hide();
	ui.mainToolBar->hide();
	statusBar()->addWidget(ui.statusFrame, 1);
	ui.statusOperationEdit->setFixedWidth(250);

	QSplitter * splitterMain{ new QSplitter(Qt::Horizontal, this) };
	splitterMain->setContentsMargins(5, 5, 5, 5);
	splitterMain->addWidget(ui.filesystemTree);
	splitterMain->addWidget(ui.statisticsGroup);
	splitterMain->setStretchFactor(0, 1);
	splitterMain->setStretchFactor(1, 3);

	setCentralWidget(splitterMain);
}

void FileSystemStatisticsWindow::updateStatus(OperationStatus status)
{
	mStatus = status;
	ui.statusOperationEdit->setText(toString(mStatus));
}

void FileSystemStatisticsWindow::clearStatistics()
{
	mStatisticsModel->clear();
	ui.statisticsSubdirsEdit->setText("0");
}

void FileSystemStatisticsWindow::clearStatus()
{
	ui.statusSelectedDirEdit->clear();
	ui.statusCurrentProcessedDirEdit->clear();
}

void FileSystemStatisticsWindow::clear()
{
	clearStatistics();
	clearStatus();
}

void FileSystemStatisticsWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
}

void FileSystemStatisticsWindow::keyPressEvent(QKeyEvent * event)
{
	QMainWindow::keyPressEvent(event);

	if (event->key() == Qt::Key_F1) {
		clear();
	}
}
