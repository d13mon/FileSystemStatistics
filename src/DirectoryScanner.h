#ifndef DIRECTORY_SCANNER_H
#define DIRECTORY_SCANNER_H

#include "ExtensionInfo.h"

#include <QObject>
#include <QRunnable>
#include <QFileInfo>
#include <atomic>

class DirectoryScanner : public QObject, public QRunnable
{
	Q_OBJECT

public:
	DirectoryScanner(QObject *parent);
	~DirectoryScanner();	

	void setDir(const QFileInfo& dirInfo);
	QFileInfo getDir() const;

	void stop();
	void reset();

protected: 
	void run() override;

private:

	void scan(const QFileInfo& dirInfo);

private:
	QFileInfo mDirInfo;

	std::atomic_bool mStopped{ false };

	ExtensionInfoHash mExtensionsInfoList;
};

#endif
