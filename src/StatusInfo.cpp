#include "StatusInfo.h"

QString toString(OperationStatus status)
{
	switch (status) {
	case OperationStatus::WAITING: return QObject::tr("DOUBLE CLICK TO CHOOSE DIRECTORY!");
	case OperationStatus::DONE: return QObject::tr("DONE.");
	case OperationStatus::PROCESSING: return QObject::tr("PROCESSING...");
	default:
		Q_ASSERT(0);
	}

	return QString{};
}
