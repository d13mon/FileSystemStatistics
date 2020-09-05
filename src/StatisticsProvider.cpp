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

	mDirInfo = dirInfo;

	mDirectoryScanner = new DirectoryScanner(dirInfo);	
	mDirectoryScanner->setAutoDelete(false);

	connect(mDirectoryScanner, &DirectoryScanner::currentProcessedDirChanged, 
		this, &StatisticsProvider::currentProcessedDirChanged, Qt::QueuedConnection);	

	connect(mDirectoryScanner, &DirectoryScanner::subdirsCountReceived, this, [this](uint count) {
		mSubdirsCount = count;
		emit subdirsCountReceived(mSubdirsCount);
	}, Qt::QueuedConnection); 

	connect(mDirectoryScanner, &DirectoryScanner::extensionsInfoAvailable,
		this, &StatisticsProvider::extensionsInfoAvailable, Qt::QueuedConnection);

	connect(mDirectoryScanner, &DirectoryScanner::finished,
		this, &StatisticsProvider::scanFinished, Qt::QueuedConnection);	

	QThreadPool::globalInstance()->start(mDirectoryScanner);
	
	emit scanStarted();
}	

void StatisticsProvider::stop()
{	
	if (mDirectoryScanner)
	{
        mDirectoryScanner->stop();		
	}	

	while (!QThreadPool::globalInstance()->waitForDone());

	//Q_ASSERT(QThreadPool::globalInstance()->activeThreadCount() == 0);

	qInfo() << TAG << "WAIT FOR DONE finished : active Threads = " 
		<< QThreadPool::globalInstance()->activeThreadCount();
	
	delete mDirectoryScanner;
	mDirectoryScanner = nullptr;	
	
	emit scanStopped();		
}

ExtensionsTotalInfo StatisticsProvider::fetchExtensionsInfo() const
{
	if (mDirectoryScanner) {
		return mDirectoryScanner->fetchExtensionsInfo();
	}

	qDebug() << TAG << "No data to fetch";

	return ExtensionsTotalInfo{};
}

uint StatisticsProvider::getSubdirsCount() const
{
	return mSubdirsCount;
}



