#include "DirectoryScanner.h"

#include <QDebug>
#include <QDir>

const QString DirectoryScanner::TAG = "DirectoryScanner:";

DirectoryScanner::DirectoryScanner(const QFileInfo& dirInfo, QObject *parent)
	: QObject(parent)	
	, mDirInfo(dirInfo)
{
}

DirectoryScanner::~DirectoryScanner()
{
	stop();
}

QFileInfo DirectoryScanner::getDir() const
{
	return mDirInfo;
}

void DirectoryScanner::run()
{	
	qDebug() << TAG << "Enter THREAD";

	if (!mDirInfo.isDir())
		return;
	
	scan(mDirInfo);	

	qDebug() << TAG  << " EXIT THREAD";
}

void DirectoryScanner::stop()
{
	mStopped = true;
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

	qDebug() << TAG << "__SCAN DIR_____: " <<  dirInfo.absoluteFilePath();

	ExtensionInfoHash extInfoHash;

	QFileInfoList files = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	for (QFileInfo fi : files) {
		if (mStopped)
			return;		

		qDebug() << fi.absoluteFilePath();

		if (fi.isDir()) {
			scan(fi);
			continue;
		}			
		
		auto ext = ExtensionInfo::parseExtension(fi);
		auto size = fi.size();	

		//qDebug() << fi.absoluteFilePath() << "EXT = " << ext << "SIZE = " + size;

		if (!extInfoHash.contains(ext)) {
			extInfoHash[ext] = { ext, 1, size };
		}
		else {
			auto extInfo = extInfoHash.take(ext);
			extInfo.filesCount++;
			extInfo.sizeBytes += size;
			extInfoHash.insert(extInfo.name, extInfo);
		}		
	}//for

	QHashIterator<QString, ExtensionInfo> i(extInfoHash);
	while (i.hasNext()) {
		i.next();
		qDebug() << i.key() << ": COUNT = " << i.value().filesCount << " SIZE = " << i.value().sizeBytes << Qt::endl;
	}
}




