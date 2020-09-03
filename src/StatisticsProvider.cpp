#include "StatisticsProvider.h"

#include "DirectoryScanner.h"

#include <QThreadPool>

StatisticsProvider::StatisticsProvider(QObject *parent)
	: QObject(parent)
{
	mDirectoryScanner = new DirectoryScanner(this); 
	mDirectoryScanner->setAutoDelete(false);
}

StatisticsProvider::~StatisticsProvider()
{
	stop();

	delete mDirectoryScanner;
}

void StatisticsProvider::start(const QFileInfo& dirInfo)
{
	stop();

	mDirectoryScanner->setDir(dirInfo);

	QThreadPool::globalInstance()->start(mDirectoryScanner);
}	

void StatisticsProvider::stop()
{	
	mDirectoryScanner->stop();

	QThreadPool::globalInstance()->waitForDone();

	mDirectoryScanner->reset();		
}
