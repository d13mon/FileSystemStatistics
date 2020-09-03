#ifndef EXTENSION_INFO_H
#define EXTENSION_INFO_H

#include <QString>
#include <QHash>

struct ExtensionInfo {
	QString name;
	uint filesCount;
	qint64 sizeBytes;
};

using ExtensionInfoHash = QHash<QString, ExtensionInfo>;

inline bool operator==(const ExtensionInfo &e1, const ExtensionInfo &e2)
{
	return e1.name == e2.name;		
}


#endif


