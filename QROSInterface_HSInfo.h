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

class QROSInterface_HSInfo : public QROSInterface_Base
{
Q_OBJECT

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
	bool m_ipNetworkAquired;

	const static QString tagServerInfo;
	const static QString tagServerPrfl;
	const static QString tagHSIPAddress;
	const static QString tagHSIPNetwork;

public:
	QROSInterface_HSInfo(ROS::Comm *mktComm, QObject *papi);

	const QString &name() const				{ return m_name; }
	void setName(const QString &name)		{ m_name = name; }

	const QString &network() const			{ return m_network;	}
	void setNetwork(const QString &network)	{ m_network = network; }

	const QString &interface() const		{ return m_ifaceName; }
	void setInterface(const QString &iface)	{ m_ifaceName = iface; }

	const QString & serverProfile() const	{ return m_profileName; }
	void setServerProfile(const QString &sp){ m_profileName = sp; }

	int addrPerMAC() const					{ return m_addrPerMAC; }
	void setAddrPerMAC(int addrPerMAC)		{ m_addrPerMAC = addrPerMAC; }

	int idleTime() const					{ return m_idleTimeout; }
	void setIdleTime(int idleTime)			{ m_idleTimeout = idleTime; }

	int keepAliveTimeout() const			{ return m_keepAliveTimeout; }
	void setKeepAliveTimeout(int kato)		{ m_keepAliveTimeout = kato; }

	int loginTimeout() const				{ return m_loginTimeout; }
	void setLoginTimeout(int lto)			{ m_loginTimeout = lto; }

	const QString &dnsName() const			{ return m_dnsName ; }
	void setDnsName(const QString &dnsName)	{ m_dnsName = dnsName; }

	const Utils::IPv4Net &hotspotNetwork() const { return m_ipNetwork; }

protected:
	void clear();
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void infoFromSentence(const ROS::QSentence &s);
	void profileFromSentence(const ROS::QSentence &s);
	void ipHSNetworkFromSentence(const ROS::QSentence &s);

signals:
	void infoReceived(const QROSInterface_HSInfo &hsInfo);
	void profileReceived(const QROSInterface_HSInfo &hsInfo);
	void allDataReceived(const QROSInterface_HSInfo &hsInfo);
};

#endif // QROSINTERFACE_HSINFO_H
