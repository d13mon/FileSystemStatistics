#ifndef STATUS_INFO_H
#define STATUS_INFO_H

#include <QString>
#include <QObject>

enum class OperationStatus { WAITING, PROCESSING, STOPPING, DONE };

QString toString(OperationStatus status);

#endif
