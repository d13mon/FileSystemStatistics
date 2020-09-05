#include "DirectoryScanner.h"

#include <QDebug>
#include <QDir>

const QString DirectoryScanner::TAG = "DirectoryScanner:";

DirectoryScanner::DirectoryScanner(const QFileInfo& dirInfo, QObject *parent)
	: QObject(parent)	
	, mDirInfo(dirInfo)
{
	mTotalExtensionInfo.name = ExtensionInfo::TOTAL_KEY;
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
	qDebug() << TAG << "RUN ENTER";

	if (!mDirInfo.isDir())
		return;

	if (auto subdirsCount = getSubdirsCount(mDirInfo); subdirsCount != 0) {
		emit subdirsCountReceived(subdirsCount);
	}
	
	scan(mDirInfo);	

	if (!mExtensionsInfoHash.empty()) {
		sendExtensionInfo();
	}

	emit finished();

	qDebug() << TAG << "RUN EXIT";
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

	emit currentProcessedDirChanged(dirInfo.absoluteFilePath());

	ExtensionInfoHash extInfoHash;

	QFileInfoList files = dir.entryInfoList(filesystemScanFilter());

	//DBG
	//qDebug() << TAG << "FilesInfo Count = " << files.size();

	for (QFileInfo fi : files) {
		if (mStopped)
			return;				

		if (fi.isDir()) {
			scan(fi);
			continue;
		}			
		
		auto ext = ExtensionInfo::parseExtension(fi);
		auto size = fi.size();		

		if (!extInfoHash.contains(ext)) {
			extInfoHash[ext] = { ext, 1, size };
		}
		else {
			auto extInfo = extInfoHash.take(ext);
			extInfo.filesCount++;
			extInfo.sizeBytes += size;
			extInfoHash.insert(extInfo.name, extInfo);
		}		
	}

	if (!extInfoHash.isEmpty()) {
		updateExtensionsInfo(extInfoHash);
	}	
}

void DirectoryScanner::updateExtensionsInfo(const ExtensionInfoHash& extInfoHash)
{
	QHashIterator<QString, ExtensionInfo> i(extInfoHash);	

	while (i.hasNext()) 
	{
		i.next();
		
		auto key = i.key();	
		auto newExtInfo = i.value();	

		std::scoped_lock lock(mExtensionsInfoMutex);

		if (!mExtensionsInfoHash.contains(key)) {
			mExtensionsInfoHash[key] = i.value();
		}
		else 
		{				
			auto extInfo = mExtensionsInfoHash.take(key);

			extInfo.filesCount += newExtInfo.filesCount;
			extInfo.sizeBytes += newExtInfo.sizeBytes;			

			mExtensionsInfoHash.insert(key, extInfo);			
		}
		
		mTotalExtensionInfo.filesCount += newExtInfo.filesCount;

		//DBG
	//	qDebug() << TAG << "TOTAL_FILES_COUNT = " << mTotalExtensionInfo.filesCount;

		mTotalExtensionInfo.sizeBytes += newExtInfo.sizeBytes;
	}		
}

void DirectoryScanner::sendExtensionInfo()
{
	auto infoList = mExtensionsInfoHash.values();	
	emit extensionsInfoAvailable({ mTotalExtensionInfo, infoList });
}

uint DirectoryScanner::getSubdirsCount(const QFileInfo& dirInfo)
{
	if (!dirInfo.isDir())
		return 0;

	QDir dir(dirInfo.absoluteFilePath());
	if (!dir.exists()) {
		qWarning() << tr("The directory is not exists: ") << dirInfo.absoluteFilePath();
		return 0;
	}

	QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
	
	return dirs.count();
}

ExtensionsTotalInfo DirectoryScanner::fetchExtensionsInfo()
{
	std::scoped_lock lock(mExtensionsInfoMutex);

	auto infoList = mExtensionsInfoHash.values();
	infoList.push_front(mTotalExtensionInfo);

	mExtensionsInfoHash.clear();

	return { mTotalExtensionInfo, std::move(infoList) };
}



