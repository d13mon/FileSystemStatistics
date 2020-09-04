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

	ExtensionInfoList getExtensionsInfo() const;

	uint getSubdirsCount() const;

signals:
	void currentProcessedDirChanged(const QString & dirPath);
	void subdirsCountReceived(uint count);
	void extensionsInfoUpdated(const ExtensionInfoList& extInfoList);

	void scanStarted();
	void scanStopped();
	void scanFinished();	

private:
	DirectoryScanner*    mDirectoryScanner = nullptr;	
	uint                 mSubdirsCount = 0;
	ExtensionInfoList    mExtensionsInfoList;
	
	static const QString TAG;
};

#endif
