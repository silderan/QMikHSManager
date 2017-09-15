#ifndef QCONFIGDATA_H
#define QCONFIGDATA_H

#include <QString>
#include "QIniFile.h"

class QConfigData
{
	QString configFName;
	QIniData iniData;

public:
	class ID : public QString
	{
	public:
		ID() : QString()
		{

		}

		ID(const char *id) : QString(id)
		{

		}

		ID(const QString &id) : QString(id)
		{

		}
	};

private:
	QConfigData::ID getFirstRouterID() const { return QConfigData::ID("1"); }
	QConfigData::ID getNextRouterID(const QConfigData::ID &id) const { return QConfigData::ID(QString("%1").arg(id.toInt()+1)); }
	QString toDataName(const QConfigData::ID &routerID, const QString &dataName) const {return QString("%1.%2").arg(routerID, dataName); }

public:

	QList<QConfigData::ID> idList;
	void defaults()
	{
		QConfigData::ID id = setName(NULL, "Exemple");
		setHost(id, "192.168.1.1");
		setPort(id, 8728);
		setUserName(id, "admin");
		setUserPass(id, "");
	}

	QConfigData() : configFName("config.ini")
	{
		defaults();
	}
	~QConfigData()
	{
		save();
	}
	QList<ID> getAllIDs() const;
	void load(const QString &fname = "") { if(!fname.isEmpty()) configFName = fname; QIniFile::load(configFName, &iniData); }
	void save() const { QIniFile::save(configFName, iniData); }

	void removeRouterData(QConfigData::ID routerID);
	QConfigData::ID firstUnusedID() const;
	QList<QConfigData::ID> getIDs() const;

	QConfigData::ID setData(QConfigData::ID routerID, const QString &dataName, const QString &dataValue);
	QString getData(const QConfigData::ID &routerID, const QString &dataName) const { return iniData[toDataName(routerID, dataName)]; }

	QConfigData::ID setName(const QConfigData::ID &routerID, const QString &name) { return setData(routerID, "hostname", name); }
	QString getName(const QConfigData::ID &routerID) const { return getData(routerID, "hostname"); }

	QConfigData::ID setHost(const QConfigData::ID &routerID, const QString &host) { return setData(routerID, "remoteHost", host); }
	QString getHost(const QConfigData::ID &routerID) const { return getData(routerID, "remoteHost"); }

	QConfigData::ID setPort(const QConfigData::ID &routerID, const quint16 &port) { return setData(routerID, "remotePort", QString("%1").arg(port)); }
	quint16 getPort(const QConfigData::ID &routerID) const { return getData(routerID, "remotePort").toUShort(); }

	QConfigData::ID setUserName(const QConfigData::ID &routerID, const QString &uname) { return setData(routerID, "username", uname);	}
	QString getUserName(const QConfigData::ID &routerID) const { return getData(routerID, "username"); }

	QConfigData::ID setUserPass(const QConfigData::ID &routerID, const QString &upass) { return setData(routerID, "userpass", upass);	}
	QString getUserPass(const QConfigData::ID &routerID) const { return getData(routerID, "userpass"); }
};

extern QConfigData globalConfig;

struct QConfigRouterData
{
	QConfigData::ID id;
	QString name;
	QString IP;
	quint16 port;
	QString uname;
	QString upass;
	QConfigRouterData()
	{

	}
};

#endif // QCONFIGDATA_H
