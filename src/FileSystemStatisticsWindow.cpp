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

	initStatisticsProvider();
	initWidgets();

	updateStatus(OperationStatus::WAITING);	

	connect(ui.filesystemTree, &QTreeView::activated, [this](const QModelIndex& index) {		
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

	connect(mStatisticsProvider, &StatisticsProvider::extensionsInfoUpdated,
		this, &FileSystemStatisticsWindow::onExtensionsInfoUpdated);

	connect(mStatisticsProvider, &StatisticsProvider::scanFinished, [this]{
		updateStatus(OperationStatus::DONE);
		clearStatus();
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
	menuBar()->hide();
	ui.mainToolBar->hide();
	ui.statisticsLogEdit->hide();

	ui.filesystemTree->setModel(mFilesystemModel);
	ui.filesystemTree->setColumnWidth(0, 250);	
	//ui.filesystemTree->setColumnHidden(1, true);
	ui.filesystemTree->setColumnHidden(2, true);
	ui.filesystemTree->setColumnHidden(3, true);	
	ui.filesystemTree->setHeaderHidden(true);	

	ui.statisticsTable->setModel(mStatisticsModel);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColFilesSize, 200);
	ui.statisticsTable->horizontalHeader()->resizeSection(StatisticsTableModel::Columns::ColAvgFileSize, 200);

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
	splitterMain->addWidget(ui.filesystemTree);
	splitterMain->addWidget(ui.statisticsGroup);
	splitterMain->setStretchFactor(0, 3);
	splitterMain->setStretchFactor(1, 3);

	setCentralWidget(splitterMain);
}

void FileSystemStatisticsWindow::startScan(const QFileInfo & dirInfo)
{
	clear();
	updateStatus(OperationStatus::PROCESSING);	
	ui.statisticsDirectoryLabel->setText("<b>" + dirInfo.absoluteFilePath() + "</b>");
	mStatisticsProvider->start(dirInfo);
}

void FileSystemStatisticsWindow::updateStatus(OperationStatus status)
{
	mStatus = status;
	ui.statusOperationEdit->setText(toString(mStatus));	

	if (status == OperationStatus::WAITING) {
		ui.statisticsDirectoryLabel->setText(toString(OperationStatus::WAITING));
	}
}

void FileSystemStatisticsWindow::onExtensionsInfoUpdated(const ExtensionInfoList& extInfoList)
{
	//qDebug() << "-----------------------------------------";

	ui.statisticsExtensionsEdit->setText(QString("%1").arg(extInfoList.size()));

	mStatisticsModel->updateData(extInfoList);

	for (auto ext : extInfoList) {
	//	qDebug() << ext.name << ": COUNT = " << ext.filesCount << " SIZE = " << ext.sizeBytes << Qt::endl;
	}
}

void FileSystemStatisticsWindow::clearStatistics()
{
	mStatisticsModel->clear();
	ui.statisticsSubdirsEdit->setText("0");
	ui.statisticsExtensionsEdit->setText("0");
	ui.statisticsLogEdit->clear();
	ui.statisticsDirectoryLabel->setText(toString(OperationStatus::WAITING));
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

void FileSystemStatisticsWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
}

void FileSystemStatisticsWindow::keyPressEvent(QKeyEvent * event)
{
	//DBG
	QMainWindow::keyPressEvent(event);

	if (event->key() == Qt::Key_F1) {
		clear();
	}
}
