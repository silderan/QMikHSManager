#ifndef QROSINTERFACE_HSINFO_H
#define QROSINTERFACE_HSINFO_H

#include "QROSInterface_Base.h"
#include "Utils.h"

union HSLoginTypes
{
	struct
	{
		uint MAC:1;
		uint Cookie:1;
		uint CHAP:1;
		uint PAP:1;
		uint MAC_Cookie:1;
		uint HTTPS:1;
		uint Trial:1;
	};
	uint allTypes;

	HSLoginTypes() : allTypes(0) { }
	HSLoginTypes(const HSLoginTypes &hslt) : allTypes(hslt.allTypes) { }
};

struct HSInfoData : public ROSInterfaceBaseData
{
	// Server info.
	QString m_name;			// Hotspot name.
	QString m_network;		// Hotspot subnet.
	QString m_ifaceName;	// Hotspot interface.
	QString m_profileName;
	QString m_poolName;
	Utils::IPv4 m_hsIP;		// Hotspot IP address.
	int m_addrPerMAC;		// Allowed addresses per MAC.
	int m_idleTimeout;		// User idle time.
	int m_keepAliveTimeout;
	int m_loginTimeout;

	// Server profile info
	QString m_dnsName;
	HSLoginTypes m_loginBy;	// Flags of login types.

	// IP HotSpot network
	Utils::IPv4Net m_ipNetwork;
};

class QROSInterface_HSInfo : public QROSInterface_Base
{
Q_OBJECT
	bool m_ipNetworkAquired;

	const static QString tagServerInfo;
	const static QString tagServerPrfl;
	const static QString tagHSIPAddress;
	const static QString tagHSIPNetwork;

public:
	QROSInterface_HSInfo(ROS::Comm *mktComm, QObject *papi);

	HSInfoData &hotspotInfoData() { return *static_cast<HSInfoData*>(rosData()); }
	const HSInfoData &hotspotInfoData() const { return *static_cast<const HSInfoData*>(rosData()); }

protected:
	void clear();
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void infoFromSentence(const ROS::QSentence &s);
	void profileFromSentence(const ROS::QSentence &s);
	void ipHSNetworkFromSentence(const ROS::QSentence &s);

signals:
	void infoReceived(const HSInfoData &hsInfo);
	void profileReceived(const HSInfoData &hsInfo);
	void allDataReceived(const HSInfoData &hsInfo);
};

#endif // QROSINTERFACE_HSINFO_H
