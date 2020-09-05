#ifndef STATISTICS_TABLE_MODEL_H
#define STATISTICS_TABLE_MODEL_H

#include "ExtensionInfo.h"

#include <QAbstractTableModel>

#include <deque>

class StatisticsTableModel : public QAbstractTableModel
{
    Q_OBJECT      

public:
	enum Columns { ColExtension, ColFilesCount, ColFilesSize, ColAvgFileSize, Count };

public:
    explicit StatisticsTableModel(QObject *parent = nullptr);
   
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;    

public:  

	void clear();    

	uint extensionsCount() const;  

	void mergeExtensionsData(const ExtensionsTotalInfo& extData );

signals:
   
	void extensionCountChanged(uint count);

private:  

	void addExtensionInfo(const ExtensionInfo& extInfo);
	void setTotalExtensionInfo(const ExtensionInfo& extInfo);

private:    
  
	ExtensionInfoList mExtensionsInfoList;
	ExtensionInfo     mTotalExtensionInfo;

	static const QString TAG;
};

#endif // STATISTICS_TABLE_MODEL
