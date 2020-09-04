#include "StatisticsProvider.h"

#include "DirectoryScanner.h"

#include <QThreadPool>
#include <QDebug>

const QString StatisticsProvider::TAG = "StatisticsProvider:";

StatisticsProvider::StatisticsProvider(QObject *parent)
	: QObject(parent)
{	
}

StatisticsProvider::~StatisticsProvider()
{
	stop();	
}

void StatisticsProvider::start(const QFileInfo& dirInfo)
{
	stop();

	Q_ASSERT(!mDirectoryScanner);

	mDirectoryScanner = new DirectoryScanner(dirInfo);	
	mDirectoryScanner->setAutoDelete(false);

	connect(mDirectoryScanner, &DirectoryScanner::currentProcessedDirChanged, 
		this, &StatisticsProvider::currentProcessedDirChanged, Qt::QueuedConnection);	

	connect(mDirectoryScanner, &DirectoryScanner::subdirsCountReceived, this, [this](uint count) {
		mSubdirsCount = count;
		emit subdirsCountReceived(mSubdirsCount);
	}, Qt::QueuedConnection); 

	connect(mDirectoryScanner, &DirectoryScanner::extensionsInfoUpdated, this, [this](const ExtensionInfoList& extInfoList) {
		mExtensionsInfoList = extInfoList;
		emit extensionsInfoUpdated(mExtensionsInfoList);
	}, Qt::QueuedConnection);

	connect(mDirectoryScanner, &DirectoryScanner::finished,
		this, &StatisticsProvider::scanFinished, Qt::QueuedConnection);

	emit scanStarted();

	QThreadPool::globalInstance()->start(mDirectoryScanner);
}	

void StatisticsProvider::stop()
{	
	qDebug() << TAG << "STOP";

	emit scanStopped();

	if (mDirectoryScanner)
	{
        mDirectoryScanner->stop();		
	}	

	qDebug() << TAG << "STOP: Waiting...";

	while (!QThreadPool::globalInstance()->waitForDone()) {

		 qDebug() << "WAIT FOR DONE...";         
	}

	qDebug() << TAG << "WAIT FOR DONE finished";
	
	delete mDirectoryScanner;
	mDirectoryScanner = nullptr;		
}

ExtensionInfoList StatisticsProvider::getExtensionsInfo() const
{
	return mExtensionsInfoList;
}

uint StatisticsProvider::getSubdirsCount() const
{
	return mSubdirsCount;
}



