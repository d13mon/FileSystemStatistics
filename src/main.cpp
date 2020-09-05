#include "FileSystemStatisticsWindow.h"
#include <QtWidgets/QApplication>

#define ENABLE_UNIT_TESTS 0

#if ENABLE_UNIT_TESTS

#include "test\runner.cpp"

#else

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

#endif
