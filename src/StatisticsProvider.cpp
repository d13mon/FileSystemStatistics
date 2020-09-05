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

	emit scanStarted();

	QThreadPool::globalInstance()->start(mDirectoryScanner);
}	

void StatisticsProvider::stop()
{	
	emit scanStopped();

	if (mDirectoryScanner)
	{
        mDirectoryScanner->stop();		
	}	

	while (!QThreadPool::globalInstance()->waitForDone());

	qDebug() << TAG << "WAIT FOR DONE finished";
	
	delete mDirectoryScanner;
	mDirectoryScanner = nullptr;		
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



