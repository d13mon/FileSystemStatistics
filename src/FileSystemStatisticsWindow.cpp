#include "FileSystemStatisticsWindow.h"
#include "StatisticsTableModel.h"
#include "DirectoryScanner.h"

#include <QSplitter>
#include <QFileSystemModel>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

#include <chrono>

using namespace std::chrono;

const QString FileSystemStatisticsWindow::TAG = "FileSystemStatisticsWindow:";

FileSystemStatisticsWindow::FileSystemStatisticsWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this); 
	
	mFilesystemModel = new QFileSystemModel;
	mFilesystemModel->setRootPath(QDir::rootPath());	
	mFilesystemModel->setFilter(DirectoryScanner::filesystemScanFilter());

	mStatisticsModel = new StatisticsTableModel(this);	
	connect(mStatisticsModel, &StatisticsTableModel::extensionCountChanged, 
		this, &FileSystemStatisticsWindow::updateExtensionsCount);	

	initStatisticsProvider();
	initWidgets();

	updateStatus(OperationStatus::WAITING);	

	connect(ui.filesystemTree, &QTreeView::clicked, [this](const QModelIndex& index) {		
		auto fi = mFilesystemModel->fileInfo(index);
		if (fi.isDir()) {
			startScan(fi);
		}
		else
		{
			QMessageBox::information(this, windowTitle(), tr("Please select a directory."), QMessageBox::Ok);
		}		
	});
}

void FileSystemStatisticsWindow::initStatisticsProvider()
{
	mStatisticsProvider = new StatisticsProvider(this);
	connect(mStatisticsProvider, &StatisticsProvider::currentProcessedDirChanged, [this](const QString & dirPath) {
		ui.statusCurrentProcessedDirEdit->setText(dirPath);			
	});
	
	connect(mStatisticsProvider, &StatisticsProvider::subdirsCountReceived, [this](uint count) {
		ui.statisticsSubdirsEdit->setText(QString("%1").arg(count));
	});

	connect(mStatisticsProvider, &StatisticsProvider::extensionsInfoAvailable,
		this, &FileSystemStatisticsWindow::onExtensionsInfoAvailable);

	connect(mStatisticsProvider, &StatisticsProvider::scanStarted, [this] {
		startUpdateStatisticsTimer();
		updateStatus(OperationStatus::PROCESSING);			
	});

	connect(mStatisticsProvider, &StatisticsProvider::scanStopped, [this] {
		updateStatus(OperationStatus::STOPPING);		
		clearStatus();
	});

	connect(mStatisticsProvider, &StatisticsProvider::scanFinished, [this]{
		stopUpdateStatisticsTimer();
		updateStatus(OperationStatus::DONE);		
		clearStatus();
	});	
}

FileSystemStatisticsWindow::~FileSystemStatisticsWindow()
{
	stopUpdateStatisticsTimer();

	delete mStatisticsProvider;
	delete mStatisticsModel;
	delete mFilesystemModel;
}

void FileSystemStatisticsWindow::initWidgets()
{
	menuBar()->hide();
	ui.mainToolBar->hide();
	ui.statisticsLogEdit->hide();
	ui.filesystemTreeHintLabel->hide();

	ui.filesystemTree->setModel(mFilesystemModel);
	ui.filesystemTree->setColumnWidth(0, 250);		
	ui.filesystemTree->setColumnHidden(2, true);
	ui.filesystemTree->setColumnHidden(3, true);	
	ui.filesystemTree->setHeaderHidden(true);	

	ui.statisticsTable->setModel(mStatisticsModel);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColNumeration, 40);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColExtension, 200);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColFilesSize, 200);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColAvgFileSize, 200);
	ui.statisticsTable->horizontalHeader()->setStyleSheet("font: bold");

	QSplitter * splitterStatistics{ new QSplitter(Qt::Horizontal, this) };
	splitterStatistics->setContentsMargins(0, 0, 0, 0);
	splitterStatistics->addWidget(ui.statisticsMainFrame);
	splitterStatistics->addWidget(ui.statisticsLogEdit);
	splitterStatistics->setStretchFactor(0, 1);
	splitterStatistics->setStretchFactor(1, 1);

	auto statisticsGroupLayout{ new QHBoxLayout };
	statisticsGroupLayout->addWidget(splitterStatistics);
	ui.statisticsGroup->setLayout(statisticsGroupLayout);	

	ui.statisticsExtensionsEdit->setFixedWidth(70);
	ui.statisticsSubdirsEdit->setFixedWidth(70);

	statusBar()->addWidget(ui.statusFrame, 1);
	ui.statusOperationEdit->setFixedWidth(250);

	QSplitter * splitterMain{ new QSplitter(Qt::Horizontal, this) };
	splitterMain->setContentsMargins(5, 5, 5, 5);
	splitterMain->addWidget(ui.filesystemFrame);
	splitterMain->addWidget(ui.statisticsGroup);
	splitterMain->setStretchFactor(0, 3);
	splitterMain->setStretchFactor(1, 3);

	setCentralWidget(splitterMain);
}

void FileSystemStatisticsWindow::startScan(const QFileInfo & dirInfo)
{
	clear();	
	ui.statisticsDirectoryLabel->setText("<b>" + dirInfo.absoluteFilePath() + "</b>");
	mStatisticsProvider->start(dirInfo);	
}

void FileSystemStatisticsWindow::updateStatus(OperationStatus status)
{
	mStatus = status;
	ui.statusOperationEdit->setText(toString(mStatus));		
}

void FileSystemStatisticsWindow::updateExtensionsCount(uint count)
{
     ui.statisticsExtensionsEdit->setText(QString("%1").arg(count));
}

void FileSystemStatisticsWindow::startUpdateStatisticsTimer()
{
	if (!mUpdateStatisticsTimerId) {
         mUpdateStatisticsTimerId = startTimer(updateStatisticsInterval());
	}	
}

void FileSystemStatisticsWindow::stopUpdateStatisticsTimer()
{
	if (mUpdateStatisticsTimerId) {
		killTimer(mUpdateStatisticsTimerId);
		mUpdateStatisticsTimerId = 0;
	}
}

std::chrono::milliseconds FileSystemStatisticsWindow::updateStatisticsInterval()
{
	return 500ms; 
}

void FileSystemStatisticsWindow::clearStatistics()
{
	mStatisticsModel->clear();
	ui.statisticsSubdirsEdit->setText("0");
	ui.statisticsExtensionsEdit->setText("0");
	ui.statisticsLogEdit->clear();
	ui.statisticsDirectoryLabel->clear();
}

void FileSystemStatisticsWindow::clearStatus()
{	
	ui.statusCurrentProcessedDirEdit->clear();
}

void FileSystemStatisticsWindow::clear()
{	
	clearStatistics();
	clearStatus();
}

void FileSystemStatisticsWindow::onExtensionsInfoAvailable(const ExtensionsTotalInfo& extInfo)
{
	updateStatisticsModelData(extInfo);
}

void FileSystemStatisticsWindow::updateStatisticsModelData(const ExtensionsTotalInfo& extInfo)
{
	if (auto & [dirPath, total, list] = extInfo;
		!dirPath.isEmpty() && dirPath == mStatisticsProvider->getCurrentDirPath()) {		
		mStatisticsModel->mergeExtensionsData(extInfo);
	}
}

void FileSystemStatisticsWindow::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == mUpdateStatisticsTimerId) {		
		auto extInfo = mStatisticsProvider->fetchExtensionsInfo();
		updateStatisticsModelData(extInfo);
	}
}



