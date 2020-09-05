#ifndef DIRECTORY_SCANNER_H
#define DIRECTORY_SCANNER_H

#include "ExtensionInfo.h"

#include <QString>
#include <QObject>
#include <QRunnable>
#include <QFileInfo>
#include <QDir>

#include <atomic>
#include <mutex>

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

	ExtensionsTotalInfo fetchExtensionsInfo();

	inline static QFlags<QDir::Filter> filesystemScanFilter() {
		return QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden;
	}	

signals:

	void currentProcessedDirChanged(const QString & dirPath);	

	void subdirsCountReceived(uint count);	

	void extensionsInfoAvailable(const ExtensionsTotalInfo& extInfo);

	void finished();	

private slots:
	void updateExtensionsInfo(const ExtensionInfoHash& extInfoHash);

	void sendExtensionsInfo();

private:

	void scan(const QFileInfo& dirInfo);

	uint getSubdirsCount(const QFileInfo& dirInfo);

	static uint getRealShortcutFileSize(const QFileInfo& fileInfo);

private:
	QFileInfo             mDirInfo;

	std::atomic_bool      mStopped{ false };

	std::mutex            mExtensionsInfoMutex;
	ExtensionInfoHash     mExtensionsInfoHash;
	ExtensionInfo         mTotalExtensionInfo;

	static const QString  TAG;
};

#endif
