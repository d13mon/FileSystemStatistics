#ifndef FILE_SYSTEM_STATISTICS_WINDOW_H
#define FILE_SYSTEM_STATISTICS_WINDOW_H

#include "ui_FileSystemStatisticsWindow.h"
#include "StatusInfo.h"
#include "StatisticsProvider.h"

#include <QtWidgets/QMainWindow>

class QFileSystemModel;
class StatisticsTableModel;

class FileSystemStatisticsWindow : public QMainWindow
{
    Q_OBJECT

public:
    FileSystemStatisticsWindow(QWidget *parent = Q_NULLPTR);	
	virtual ~FileSystemStatisticsWindow();

public slots:
	
	void startScan(const QFileInfo & dirInfo);
	
	void clear();	

private slots:	

	void onExtensionsInfoAvailable(const ExtensionsTotalInfo& extInfo);
	
	void updateExtensionsCount(uint count);

private:
	void initWidgets();
	void initStatisticsProvider();

	void clearStatistics();
	void clearStatus();

	void updateStatus(OperationStatus status);	

	void startUpdateStatisticsTimer();
	void stopUpdateStatisticsTimer();

	static std::chrono::milliseconds updateStatisticsTime();	

protected: 
	virtual void closeEvent(QCloseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void timerEvent(QTimerEvent* event) override;

private:
    Ui::FileSystemStatisticsWindowClass ui;

	QFileSystemModel*                   mFilesystemModel = nullptr;

	OperationStatus                     mStatus = OperationStatus::WAITING;

	StatisticsProvider*                 mStatisticsProvider = nullptr;

	StatisticsTableModel*               mStatisticsModel = nullptr;	

	int                                 mUpdateStatisticsTimerId = 0;

	static const QString                TAG;
};

#endif // FILE_SYSTEM_STATISTICS_WINDOW_H