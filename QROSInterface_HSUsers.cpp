#include "QROSInterface_HSUsers.h"
#include "Utils.h"

const QString QROSInterface_HSUsers::tagGetallData = QString("G_HSUser");
const QString QROSInterface_HSUsers::tagListenData = QString("L_HSUser");

QROSInterface_HSUsers::QROSInterface_HSUsers(ROS::Comm *mktComm, QObject *papi) :
	QROSInterface_HSUBase(mktComm, new HSUserData(), papi)
{
	addValidVersions(QROSVersion("5.0.0"), QROSVersion("99.99.99"));
}

bool QROSInterface_HSUsers::doRequest()
{
	switch( versionIndex() )
	{
	case 0:
		deployingInitialData = true;
		mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/user/getall", tagGetallData ) );
		mktComm()->sendSentence( ROS::QSentence( "/ip/hotspot/user/listen", tagListenData ) );
		return true;
	}
	return false;
}

bool QROSInterface_HSUsers::parseResponse(const ROS::QSentence &s)
{
	if( s.tag() == tagGetallData )
	{
		if( s.getResultType() == ROS::QSentence::Done )	// End.
		{
			deployingInitialData = false;
			for( int i = 0; i < queuedSentence.count(); i++ )
				parseResponse(queuedSentence[i]);
			queuedSentence.clear();
			emit allDataReceived( *(static_cast<HSUserData*>(rosData())) );
		}
		else
		{
			fromSentence(s);
			emit dataReceived( *(static_cast<HSUserData*>(rosData())), true );
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
				emit dataDeleted( *(static_cast<HSUserData*>(rosData())) );
			else
				emit dataReceived( *(static_cast<HSUserData*>(rosData())), false );
		}
	}
	else
		return false;
	return true;
}

void QROSInterface_HSUsers::fromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		static_cast<HSUserData*>(rosData())->m_userName = s.attribute("name");
		static_cast<HSUserData*>(rosData())->m_downloadedBits = BYTES_TO_BITS( s.attribute("bytes-out").toULongLong() );
		static_cast<HSUserData*>(rosData())->m_uploadedBits = BYTES_TO_BITS( s.attribute("bytes-in").toULongLong() );
		static_cast<HSUserData*>(rosData())->m_uptime = s.attribute("uptime");

		static_cast<HSUserData*>(rosData())->m_userPass = s.attribute("password");
		break;
	}
}
