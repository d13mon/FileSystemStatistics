#include "FileSystemStatisticsWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	qRegisterMetaType<ExtensionInfo>("ExtensionInfo");
	qRegisterMetaType<ExtensionInfoList>("ExtensionInfoList");
	qRegisterMetaType<ExtensionInfoHash>("ExtensionInfoHash");
	qRegisterMetaType<ExtensionsTotalInfo>("ExtensionsTotalInfo");

    QApplication a(argc, argv);
    FileSystemStatisticsWindow w;
    w.show();
    return a.exec();
}


