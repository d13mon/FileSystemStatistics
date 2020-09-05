#ifndef EXTENSION_INFO_H
#define EXTENSION_INFO_H

#include <QString>
#include <QHash>
#include <QList>
#include <QFileInfo>

struct ExtensionInfo {
	QString name;
	uint filesCount = 0;
	qint64 sizeBytes = 0;
	static const QString TOTAL_KEY;

	inline bool isTotal() const { return name == TOTAL_KEY; }	

	inline qint64 getAvgFileSize() {
		return filesCount > 0 ? (sizeBytes / filesCount) : 0;
	}	

	static QString parseExtension(const QFileInfo& file);
};


using ExtensionInfoHash = QHash<QString, ExtensionInfo>;
using ExtensionInfoList = QList<ExtensionInfo>;
using ExtensionsTotalInfo = std::pair<ExtensionInfo, ExtensionInfoList>;

inline bool operator==(const ExtensionInfo &e1, const ExtensionInfo &e2)
{
	return e1.name == e2.name;		
}

inline bool operator<(const ExtensionInfo &e1, const ExtensionInfo &e2)
{
	return e1.name < e2.name;
}


#endif


