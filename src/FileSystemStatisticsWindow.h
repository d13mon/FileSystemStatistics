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

private:
	void initWidgets();
	void initStatisticsProvider();
	
	void clearStatistics();
	void clearStatus();

	void updateStatus(OperationStatus status);

	inline uint maxScanStatusMessageCount() const { return 10; }

private slots:	

	void onExtensionsInfoUpdated(const ExtensionInfoList& extInfoList);

protected: 
	virtual void closeEvent(QCloseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

private:
    Ui::FileSystemStatisticsWindowClass ui;

	QFileSystemModel*                   mFilesystemModel = nullptr;

	OperationStatus                     mStatus = OperationStatus::WAITING;

	StatisticsProvider*                 mStatisticsProvider = nullptr;

	StatisticsTableModel*               mStatisticsModel = nullptr;	
};

#endif // FILE_SYSTEM_STATISTICS_WINDOW_H