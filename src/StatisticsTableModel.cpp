#include "StatisticsTableModel.h"

#include <QDebug>

const QString StatisticsTableModel::TAG = "StatisticsTableModel:";

StatisticsTableModel::StatisticsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{   
	//dbgFill();
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

    return getRowCount();
}

int StatisticsTableModel::getRowCount() const
{
    return mStatsList.size();
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
   
    if(row >= getRowCount() )
        return QString();  

    switch (role) {
        case Qt::DisplayRole:
        { 
			auto extInfo = mStatsList[row];		 

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

bool StatisticsTableModel::update(int modeIndex)
{
    //clear();

	//beginInsertRows(QModelIndex(), 0, scores.size());

	//std::copy(std::begin(scores), std::end(scores), std::back_inserter(scores_));

	//endInsertRows();

	//emit dataChanged(createIndex(0, 0), createIndex(getRowCount() - 1, columnCount() - 1));    

    return true;
}

void StatisticsTableModel::clear()
{ 
	beginResetModel();

	mStatsList.clear();

	endResetModel();	  
}

void StatisticsTableModel::dbgFill()
{
	mStatsList.push_back({ "txt", 45, 4565435 });
	mStatsList.push_back({ "exe", 5, 4554});
	mStatsList.push_back({ "zip", 58, 5676797896 });
}


