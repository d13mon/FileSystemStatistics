#include "StatusInfo.h"

QString toString(OperationStatus status)
{
	switch (status) {
	case OperationStatus::WAITING: return QObject::tr("WAITING...");
	case OperationStatus::PROCESSING: return QObject::tr("PROCESSING...");
	case OperationStatus::STOPPING: return QObject::tr("STOPPING...");
	case OperationStatus::DONE: return QObject::tr("DONE.");

	default:
		Q_ASSERT(0);
	}

	return QString{};
}
