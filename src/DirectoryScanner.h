#ifndef DIRECTORY_SCANNER_H
#define DIRECTORY_SCANNER_H

#include "ExtensionInfo.h"

#include <QString>
#include <QObject>
#include <QRunnable>
#include <QFileInfo>
#include <atomic>

class DirectoryScanner : public QObject, public QRunnable
{
	Q_OBJECT

public:
	DirectoryScanner(const QFileInfo& dirInfo, QObject *parent = nullptr);
	~DirectoryScanner();	
	
	QFileInfo getDir() const;

public:
    void run() override;
	void stop();	

signals:

	void currentProcessedDirChanged(const QString & dirPath);	

private:

	void scan(const QFileInfo& dirInfo);

private:
	QFileInfo mDirInfo;

	std::atomic_bool mStopped{ false };

	//ExtensionInfoHash mExtensionsInfoList;

	static const QString TAG;
};

#endif
