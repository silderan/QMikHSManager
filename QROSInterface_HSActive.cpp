#include "QROSInterface_HSActive.h"
#include "Utils.h"

const QString QROSInterface_HSActive::tagGetallData = QString("G_HSActive");
const QString QROSInterface_HSActive::tagListenData = QString("L_HSActive");

QROSInterface_HSActive::QROSInterface_HSActive(ROS::Comm *mktComm, QObject *papi) :
	QROSInterface_HSUBase(mktComm, new HSActiveUser(), papi)
{
	addValidVersions(QROSVersion("5.0.0"), QROSVersion("99.99.99"));
}

bool QROSInterface_HSActive::doRequest()
{
	switch( versionIndex() )
	{
	case 0:
		deployingInitialData = true;
		mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/active/getall", tagGetallData ) );
		mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/active/listen", tagListenData ) );
		return true;
	}
	return false;
}

bool QROSInterface_HSActive::parseResponse(const ROS::QSentence &s)
{
	if( s.tag() == tagGetallData )
	{
		if( s.getResultType() == ROS::QSentence::Done )	// End.
		{
			deployingInitialData = false;
			for(int i = 0; i < queuedSentence.count(); i++ )
				parseResponse(queuedSentence[i]);
			queuedSentence.clear();
		}
		else
		{
			fromSentence(s);
			emit dataReceived( *(static_cast<HSActiveUser*>(rosData())), true);
		}
	}
	else
	if( s.tag() == tagListenData )
	{
		if( deployingInitialData )
			queuedSentence.append(s);
		else
		{
			fromSentence(s);
			if( !s.attribute(".dead").isEmpty() )	// Deleting
				emit dataDeleted( *(static_cast<HSActiveUser*>(rosData())) );
			else
				emit dataReceived( *(static_cast<HSActiveUser*>(rosData())), false);
		}
	}
	else
		return false;
	return true;
}

void QROSInterface_HSActive::fromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		static_cast<HSActiveUser*>(rosData())->m_userName = s.attribute("user");
		static_cast<HSActiveUser*>(rosData())->m_downloadedBits = BYTES_TO_BITS( s.attribute("bytes-out").toULongLong() );
		static_cast<HSActiveUser*>(rosData())->m_uploadedBits = BYTES_TO_BITS( s.attribute("bytes-in").toULongLong() );
		static_cast<HSActiveUser*>(rosData())->m_uptime = s.attribute("uptime");

		static_cast<HSActiveUser*>(rosData())->m_IP = s.attribute("address");
		static_cast<HSActiveUser*>(rosData())->m_MAC = s.attribute("mac-address");
		break;
	}
}
