#include "DirectoryScanner.h"

#include <QDebug>
#include <QDir>


DirectoryScanner::DirectoryScanner(QObject *parent)
	: QObject(parent)	
{
}

DirectoryScanner::~DirectoryScanner()
{
	stop();
}

void DirectoryScanner::setDir(const QFileInfo& dirInfo)
{
	mDirInfo = dirInfo;
}

QFileInfo DirectoryScanner::getDir() const
{
	return mDirInfo;
}

void DirectoryScanner::run()
{	
	qDebug() << "RUN";

	if (!mDirInfo.isDir())
		return;
	
	scan(mDirInfo);
}

void DirectoryScanner::scan(const QFileInfo& dirInfo)
{
	if (mStopped || !dirInfo.isDir())
		return;	

	QDir dir(dirInfo.absoluteFilePath());
	if (!dir.exists()) {
		qWarning() << tr("The directory is not exists: ") << dirInfo.absoluteFilePath();
		return;
	}

	QFileInfoList files = dir.entryInfoList();
	for (QFileInfo fi : files) {
		if (mStopped)
			return;

		qDebug() << fi.absoluteFilePath();
	}
}

void DirectoryScanner::stop()
{
	mStopped = true;
}

void DirectoryScanner::reset()
{
	mExtensionsInfoList.clear();
	mDirInfo = QFileInfo{};

	mStopped = false;
}
