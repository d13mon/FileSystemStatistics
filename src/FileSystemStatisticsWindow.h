#ifndef FILE_SYSTEM_STATISTICS_WINDOW_H
#define FILE_SYSTEM_STATISTICS_WINDOW_H

#include "ui_FileSystemStatisticsWindow.h"
#include "StatusInfo.h"
#include "StatisticsProvider.h"

#include <QtWidgets/QMainWindow>

class QFileSystemModel;

class FileSystemStatisticsWindow : public QMainWindow
{
    Q_OBJECT

public:
    FileSystemStatisticsWindow(QWidget *parent = Q_NULLPTR);
	virtual ~FileSystemStatisticsWindow();
private:
	void initWidgets();

	void updateStatus(OperationStatus status);

protected: 
	virtual void closeEvent(QCloseEvent* event) override;

private:
    Ui::FileSystemStatisticsWindowClass ui;

	QFileSystemModel* mFilesystemModel = nullptr;

	OperationStatus mStatus = OperationStatus::WAITING;

	StatisticsProvider* mStatisticsProvider = nullptr;
};

#endif // FILE_SYSTEM_STATISTICS_WINDOW_H