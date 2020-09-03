#ifndef STATISTICS_PROVIDER_H
#define STATISTICS_PROVIDER_H

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
private:
	DirectoryScanner* mDirectoryScanner = nullptr;

	static const QString TAG;
};

#endif
