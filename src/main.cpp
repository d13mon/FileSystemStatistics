#include "FileSystemStatisticsWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileSystemStatisticsWindow w;
    w.show();
    return a.exec();
}
