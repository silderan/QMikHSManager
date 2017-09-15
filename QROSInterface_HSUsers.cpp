#include "QROSInterface_HSUsers.h"

const QString QROSInterface_HSUsers::tagGetallData = QString("G_HSUser");
const QString QROSInterface_HSUsers::tagListenData = QString("L_HSUser");

QROSInterface_HSUsers::QROSInterface_HSUsers(ROS::Comm *mktComm, QObject *papi) :
	QROSInterface_Base(mktComm, papi),
	deployingInitialData(false)
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
			emit allDataReceived();
		}
		else
		{
			fromSentence(s);
			emit dataReceived(*this, true);
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
				emit dataDeleted(*this);
			else
				emit dataReceived(*this, false);
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
		m_userName = s.attribute("name");
		m_userPass = s.attribute("password");
		m_downloaded = s.attribute("bytes-out").toULongLong();
		m_uploaded = s.attribute("bytes-in").toULongLong();
		m_uptime = s.attribute("uptime");
		break;
	}
}
