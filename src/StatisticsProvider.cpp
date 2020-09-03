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

	QThreadPool::globalInstance()->start(mDirectoryScanner);
}	

void StatisticsProvider::stop()
{	
	qDebug() << TAG << "STOP";

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
