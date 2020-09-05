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
	qInfo() << TAG << "RUN ENTER: dir =" << mDirInfo.absoluteFilePath() << " | is_dir = " << mDirInfo.isDir();		
	
	if (auto subdirsCount = getSubdirsCount(mDirInfo); subdirsCount != 0) {
		emit subdirsCountReceived(subdirsCount);
	}
	
	scan(mDirInfo);	

	if (!mStopped) {
		sendExtensionsInfo();
		emit finished();
	}	

	qInfo() << TAG << "RUN EXIT: dir = " << mDirInfo.absoluteFilePath() << " STOPPED = " << mStopped;
}

void DirectoryScanner::stop()
{
	mStopped = true;
}

void DirectoryScanner::scan(const QFileInfo& dirInfo)
{
	if (mStopped)
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

	//qInfo() << TAG << "SCAN ENTER: DIR = " << dirInfo.absoluteFilePath();

	for (QFileInfo fi : files) {
		if (mStopped)
			return;				

		if (fi.isDir()) {
			scan(fi);
			continue;
		}			
		
		auto ext = ExtensionInfo::parseExtension(fi);
		auto size = fi.size();		

		if (fi.isShortcut()) {
			size = getRealShortcutFileSize(fi);
		}

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

	//qInfo() << TAG << "SCAN EXIT: DIR = " << dirInfo.absoluteFilePath() << " | results ready: size = " << extInfoHash.size();

	if (!extInfoHash.isEmpty()) {
		updateExtensionsInfo(extInfoHash);
	}	
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

uint DirectoryScanner::getRealShortcutFileSize(const QFileInfo& fileInfo)
{
	uint size = 0;
	QFile file(fileInfo.absoluteFilePath());
	if (file.exists() && file.open(QIODevice::ReadOnly)) {
		//NOTE: Now the file size shows correctly
		size = file.size();
		file.close();
	}

	return size;
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
        mTotalExtensionInfo.sizeBytes += newExtInfo.sizeBytes;	
	}		
}

void DirectoryScanner::sendExtensionsInfo()
{
	std::scoped_lock lock(mExtensionsInfoMutex);
	
	auto infoList = mExtensionsInfoHash.values();
	emit extensionsInfoAvailable({ mDirInfo.absoluteFilePath(), mTotalExtensionInfo, infoList });	
}

ExtensionsTotalInfo DirectoryScanner::fetchExtensionsInfo()
{
	std::scoped_lock lock(mExtensionsInfoMutex);

//	qInfo() << TAG << "FETCH DATA: size =" << mExtensionsInfoHash.size();

	auto infoList = mExtensionsInfoHash.values();

	mExtensionsInfoHash.clear();

	return { mDirInfo.absoluteFilePath(), mTotalExtensionInfo, std::move(infoList) };
}






