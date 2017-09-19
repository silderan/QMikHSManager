#include "QROSInterface_HSInfo.h"

const QString QROSInterface_HSInfo::tagServerInfo = QString("G_HSServerInfo");
const QString QROSInterface_HSInfo::tagServerPrfl = QString("G_HSServerPrfl");
const QString QROSInterface_HSInfo::tagHSIPAddress = QString("G_HSIPAddress");
const QString QROSInterface_HSInfo::tagHSIPNetwork = QString("G_HSIPNetwork");

QROSInterface_HSInfo::QROSInterface_HSInfo(ROS::Comm *mktComm, QObject *papi) :
	QROSInterface_Base(mktComm, new HSInfoData(), papi)
{
	addValidVersions("4.0.0", "99.99.99");
}

void QROSInterface_HSInfo::clear()
{
	hotspotInfoData().m_name.clear();
	hotspotInfoData().m_network.clear();
	hotspotInfoData().m_ifaceName.clear();
	hotspotInfoData().m_profileName.clear();
	hotspotInfoData().m_dnsName.clear();
	m_ipNetworkAquired = false;
}

bool QROSInterface_HSInfo::doRequest()
{
	// Requesting hotspot info involves three separated info:
	// First: Server configuration itself.
	// Second: Server profile used.
	// Third: Address pool used by hotspot (known looking at hotspot interface address network)
	// But, for retrieveing one, the previous one is requiered,
	// So, in this function, just the first step could be done and the other will be requested later.
	switch( versionIndex() )
	{
	case 0:
		clear();
		mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/getall", tagServerInfo ) );
		return true;
	}
	return false;
}

bool QROSInterface_HSInfo::parseResponse(const ROS::QSentence &s)
{
	// Step 1
	if( s.tag() == tagServerInfo )
	{
		if( s.getResultType() == ROS::QSentence::Done )	// End.
		{
			if( hotspotInfoData().m_profileName.isEmpty() )
			{
				setError(QROSInterfaceError::ROSBadConfig, tr("Sorry, the router has more that one hotspot server.\n"
															  "Sadly, this program cannot create new hotspot setup for you.\n"
															  "Please, configure a runing hotspot and reconect to router after."));
			}
			else
			{
				emit infoReceived(hotspotInfoData());
				mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/profile/getall", tagServerPrfl ) );
			}
		}
		else if( s.getResultType() == ROS::QSentence::Reply )
		{
			if( !hotspotInfoData().m_profileName.isEmpty() )
				setError(QROSInterfaceError::ROSBadConfig, tr("Sorry, the router has more that one hotspot server.\n"
															  "Sadly, this program is not designet to handled this.\n"
															  "Please, remove one of them and reconect to router later on."));
			else
				infoFromSentence(s);
		}
		return true;
	}
	// Step 2
	else if( s.tag() == tagServerPrfl )
	{
		if( s.getResultType() == ROS::QSentence::Done )	// End.
		{
			if( (int)hotspotInfoData().m_hsIP == (int)0 )
				setError(QROSInterfaceError::ROSBadConfig, tr("Sorry, the hotspot profile (%1) doesn't have defined any hotspot-address.\n"
															  "Sadly, this program is not designet to handled this.\n"
															  "Please, configure hotspot properly and reconect to router later on.").arg(hotspotInfoData().m_profileName));
			else
				mktComm()->sendSentence( ROS::QSentence( "/ip/address/getall", tagHSIPAddress ) );
		}
		else
		if( s.getResultType() == ROS::QSentence::Reply )
			profileFromSentence(s);
		return true;
	}
	else if( s.tag() == tagHSIPAddress )
	{
		if( s.getResultType() == ROS::QSentence::Done )	// End.
		{
			if( !hotspotInfoData().m_ipNetwork.inRange(hotspotInfoData().m_hsIP) )
				setError(QROSInterfaceError::ROSBadConfig, tr("Sorry, there is no valid addres network on the hotspot interface (%1).\n"
															  "Sadly, this program is not designet to handled this.\n"
															  "Please, set a valid address network on this interface or \n"
															  "modify the hotspot-address to fit into the address netkork.").arg(hotspotInfoData().m_ifaceName));
			else
				emit allDataReceived(hotspotInfoData());
		}
		else
		if( s.getResultType() == ROS::QSentence::Reply )
			ipHSNetworkFromSentence(s);
		return true;
	}
	else
		return false;
	return true;
}

void QROSInterface_HSInfo::profileFromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		if( s.attribute("name") == hotspotInfoData().m_profileName )
		{
			hotspotInfoData().m_dnsName = s.attribute("dns-name");
			if( hotspotInfoData().m_dnsName.isEmpty() )
				hotspotInfoData().m_dnsName = "<Undefined>";
			foreach( QString s, s.attribute("login-by").split(",") )
			{
				if( s == "mac" )				hotspotInfoData().m_loginBy.MAC = 1;
				else if( s == "cookie" )		hotspotInfoData().m_loginBy.Cookie = 1;
				else if( s == "http-chap" )		hotspotInfoData().m_loginBy.CHAP = 1;
				else if( s == "https" )			hotspotInfoData().m_loginBy.HTTPS = 1;
				else if( s == "http-pap" )		hotspotInfoData().m_loginBy.PAP = 1;
				else if( s == "trial" )			hotspotInfoData().m_loginBy.Trial = 1;
				else if( s == "mac-cookie" )	hotspotInfoData().m_loginBy.MAC_Cookie = 1;
			}
			hotspotInfoData().m_hsIP.fromString(s.attribute("hotspot-address"));
		}
		break;
	}
}

void QROSInterface_HSInfo::ipHSNetworkFromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		if( (s.attribute("interface") == hotspotInfoData().m_ifaceName) )
		{
			Utils::IPv4Net tempIP(s.attribute("address"));
			if( tempIP.inRange(hotspotInfoData().m_hsIP) )
				hotspotInfoData().m_ipNetwork = tempIP;
		}
		break;
	}
}

void QROSInterface_HSInfo::infoFromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		hotspotInfoData().m_name				= s.attribute("name");
		hotspotInfoData().m_ifaceName			= s.attribute("interface");
		hotspotInfoData().m_poolName			= s.attribute("address-pool");
		hotspotInfoData().m_profileName			= s.attribute("profile");
		hotspotInfoData().m_addrPerMAC			= s.attribute("addresses-per-mac").toInt();
		hotspotInfoData().m_idleTimeout			= Utils::rosTimeToInt(s.attribute("idle-timeout"));
		hotspotInfoData().m_keepAliveTimeout	= Utils::rosTimeToInt(s.attribute("keepalive-timeout"));
		hotspotInfoData().m_loginTimeout		= Utils::rosTimeToInt(s.attribute("login-timeout"));
		break;
	}
}
