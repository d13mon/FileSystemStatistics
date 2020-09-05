#ifndef STATISTICS_PROVIDER_H
#define STATISTICS_PROVIDER_H

#include "ExtensionInfo.h"

#include <QObject>
#include <QFileInfo>

class DirectoryScanner;

class StatisticsProvider : public QObject
{
	Q_OBJECT

public:
	StatisticsProvider(QObject *parent);
	virtual ~StatisticsProvider();

	void start(const QFileInfo& dirInfo);
	void stop();

	ExtensionsTotalInfo fetchExtensionsInfo() const;

	uint getSubdirsCount() const;

	inline QFileInfo getCurrentDirInfo() const { return mDirInfo; };
	inline QString getCurrentDirPath() const { return mDirInfo.absoluteFilePath(); };

signals:
	void currentProcessedDirChanged(const QString & dirPath);
	void subdirsCountReceived(uint count);
	void extensionsInfoAvailable(const ExtensionsTotalInfo& extInfo);

	void scanStarted();
	void scanStopped();
	void scanFinished();	

private:
	DirectoryScanner*    mDirectoryScanner = nullptr;	
	uint                 mSubdirsCount = 0;	
	QFileInfo            mDirInfo;
	
	static const QString TAG;
};

#endif
