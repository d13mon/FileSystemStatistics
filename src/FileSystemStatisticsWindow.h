#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileSystemStatisticsWindow.h"

class FileSystemStatisticsWindow : public QMainWindow
{
    Q_OBJECT

public:
    FileSystemStatisticsWindow(QWidget *parent = Q_NULLPTR);

private:
	void initWidgets();

private:
    Ui::FileSystemStatisticsWindowClass ui;
};
