#include "StatisticsTableModel.h"

#include <QDebug>
#include <QCoreApplication>

#include<algorithm>

const QString StatisticsTableModel::TAG = "StatisticsTableModel:";

StatisticsTableModel::StatisticsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{   
	
}

QVariant StatisticsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(orientation);

	switch (role) {
	case Qt::DisplayRole:
		switch (section)
		{
		case ColExtension:  return tr("Extension");
		case ColFilesCount: return tr("Count");
		case ColFilesSize: return tr("Total size (Bytes)");
		case ColAvgFileSize: return tr("AVG size (Bytes)");
		default:
			return QString();
		}
	default:
		break;
	}

    return QVariant();
}

int StatisticsTableModel::rowCount(const QModelIndex &parent) const
{
	//NOTE: When implementing a table based model, rowCount() should return 0 when the parent is valid.
    if (parent.isValid())
        return 0;

    return mExtensionsInfoList.size() + (mTotalExtensionInfo.isTotal() ? 1 : 0);
}

int StatisticsTableModel::columnCount(const QModelIndex &parent) const
{
	//NOTE: When implementing a table based model, columnCount() should return 0 when the parent is valid.
    if (parent.isValid())
        return 0;

   return Columns::Count;
}

QVariant StatisticsTableModel::data(const QModelIndex &index, int role) const
{	
    if (!index.isValid())
        return QString();

    auto row  = index.row();  
   
    if(row >= rowCount() )
        return QString();  

    switch (role) {
        case Qt::DisplayRole:
        { 
			auto extInfo = row == 0 ? mTotalExtensionInfo : mExtensionsInfoList[row-1];		 

            switch(index.column())
            {
			case ColExtension:  return extInfo.name.toUpper();
			case ColFilesCount: return extInfo.filesCount;
			case ColFilesSize: return extInfo.sizeBytes;
			case ColAvgFileSize: return extInfo.getAvgFileSize();
			default:
				return QString();
            }
        }
        break; 

		case Qt::TextAlignmentRole:
			switch (index.column())
			{
			case ColExtension:  return Qt::AlignCenter;
			case ColFilesCount: return Qt::AlignCenter;
			case ColFilesSize: return QVariant(Qt::AlignRight | Qt::AlignVCenter);
			case ColAvgFileSize: return QVariant(Qt::AlignRight | Qt::AlignVCenter);
			default:
				return QString();
			}
			break;

        default:
            break;
    }

    return QString();
}

void StatisticsTableModel::mergeExtensionsData(const ExtensionsTotalInfo& extData)
{	
	qDebug() << TAG << "MERGE: rowCount = " << rowCount();

	setTotalExtensionInfo(extData.first);

	for (auto extInfo : extData.second) {		
         addExtensionInfo(extInfo);	

		//QCoreApplication::processEvents();
	}	

	emit extensionCountChanged(extensionsCount());
}

void StatisticsTableModel::clear()
{ 
	beginResetModel();

	mExtensionsInfoList.clear();
	mTotalExtensionInfo = ExtensionInfo{};

	endResetModel();

	emit extensionCountChanged(extensionsCount());
}

uint StatisticsTableModel::extensionsCount() const
{	
	return mExtensionsInfoList.size();
}

void StatisticsTableModel::addExtensionInfo(const ExtensionInfo& extInfo)
{
	auto ipos = std::lower_bound(std::begin(mExtensionsInfoList), std::end(mExtensionsInfoList), extInfo);	

	qDebug() << TAG << "----->ENTER ADD EXT INFO: " << extInfo.name << " Files = " << extInfo.filesCount << " ROWS = " << rowCount();
	
	int index = 0;

	if (ipos != std::end(mExtensionsInfoList)) {
		
		//NOTE: TOTAL info is always on first row (index = 0)	
		int index = std::distance(std::begin(mExtensionsInfoList), ipos) + 1;	

		if (extInfo.name == ipos->name) {
			
			auto oldFilesCount = ipos->filesCount;
			ipos->sizeBytes += extInfo.sizeBytes;
			ipos->filesCount += extInfo.filesCount;
			qDebug() << TAG << "UPDATED: " << ipos->name << " Files = " << ipos->filesCount << "(+" << ipos->filesCount - oldFilesCount << ")";
		}
		else
		{
			qDebug() << TAG << "INSERTED(" << index << "): " << ipos->name << " Files = " << extInfo.filesCount;
			beginInsertRows(QModelIndex(), index, index);			
			mExtensionsInfoList.insert(ipos, extInfo);	
			endInsertRows();		
		}		
	}
	else {
		//NOTE: TOTAL info is always on first row (index = 0)
		index = mExtensionsInfoList.size() + 1;
		
		qDebug() << TAG << "ADDED BACK(" << index  << "): " << extInfo.name << " Files = " << extInfo.filesCount;

        beginInsertRows(QModelIndex(), index, index);
		mExtensionsInfoList.push_back(extInfo);
		endInsertRows();
		
	}	

	//emit dataChanged(createIndex(index, 0), createIndex(index, columnCount() - 1));

	qDebug() << TAG << "<------EXIT ADD EXT INFO: " << extInfo.name << " ROWS = " << rowCount() << " Extensions = " << extensionsCount();
	
	Q_ASSERT(rowCount() == mExtensionsInfoList.size() + 1);	
}

void StatisticsTableModel::setTotalExtensionInfo(const ExtensionInfo& extInfo)
{
	if (!mTotalExtensionInfo.isTotal()) {		
		beginInsertRows(QModelIndex(), 0, 0);
		mTotalExtensionInfo = extInfo;	    
		endInsertRows();
		Q_ASSERT(mTotalExtensionInfo.isTotal());
		qDebug() << TAG << "TOTAL INSERTED: " << extInfo.name << " Files = " << extInfo.filesCount << " rowCount = " << rowCount();
		return;
	}

	qDebug() << TAG << "TOTAL UPDATED: " << extInfo.name << " Files = " << extInfo.filesCount;

	mTotalExtensionInfo = extInfo;
}




