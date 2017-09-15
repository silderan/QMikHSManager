#include "qconfigdata.h"
#include "../QMikrotikAPI/QIniFile.h"
#include <QStringList>

QConfigData globalConfig;


/**
 * @brief QConfigData::removeRouterData
 * remove all data from config.
 * To do so, it looks entire map keys and every key starting with <routerID>+"." will be deleted.
 * @param routerID the router ID to remove from mapping.
 */
void QConfigData::removeRouterData(QConfigData::ID routerID)
{
	routerID = routerID.append(".");
	foreach( QString key, iniData.keys() )
	{
		if( key.startsWith( routerID) )
			iniData.remove(key);
	}
}

QList<QConfigData::ID> QConfigData::getAllIDs() const
{
	QList<QConfigData::ID> list;
	foreach( QString key, iniData.keys() )
	{
		QConfigData::ID id = key.left(key.indexOf('.'));
		if( !list.contains(id) )
			list.append(id);
	}

	return list;
}

/**
 * @brief QConfigData::firstUnusedID
 * Search for the first unused ID
 * The returned value must be used inmediately becouse subsequents calls could use the same ID
 * @return the first unused ID
 */
QConfigData::ID QConfigData::firstUnusedID() const
{
	QList<QConfigData::ID> idList = getAllIDs();
	QConfigData::ID id = getFirstRouterID();
	while( idList.contains(id) )
		id = getNextRouterID(id);

	return id;
}

QList<QConfigData::ID> QConfigData::getIDs() const
{
	QList<QConfigData::ID> list;
	foreach( QConfigData::ID k, iniData.keys() )
	{
		k = k.split(QChar('.')).first();
		if( !list.contains(k) )
			list.append(k);
	}
	return list;
}

/**
 * @brief QConfigData::setData
 * Appends or updates data in mapping
 * @param routerID the router ID.
 * @param dataName the arbitrary name wich stores and identifies the data.
 * @param dataValue the data value.
 */
QConfigData::ID QConfigData::setData(ID routerID, const QString &dataName, const QString &dataValue)
{
	if( routerID.isEmpty() )
		routerID = firstUnusedID();
	iniData[toDataName(routerID, dataName)] = dataValue;
	return routerID;
}
