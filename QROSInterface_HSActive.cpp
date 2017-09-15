#include "QROSInterface_HSActive.h"

const QString QROSInterface_HSActive::tagGetallData = QString("G_HSActive");
const QString QROSInterface_HSActive::tagListenData = QString("L_HSActive");

QROSInterface_HSActive::QROSInterface_HSActive(ROS::Comm *mktComm, QObject *papi) :
	QROSInterface_Base(mktComm, papi),
	deployingInitialData(false)
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

void QROSInterface_HSActive::fromSentence(const ROS::QSentence &s)
{
	switch( versionIndex() )
	{
	case 0:
		m_userName = s.attribute("user");
		m_downloaded = s.attribute("bytes-out").toULongLong();
		m_uploaded = s.attribute("bytes-in").toULongLong();
		m_uptime = s.attribute("uptime");
		m_IP = s.attribute("address");
		m_MAC = s.attribute("mac-address");
		break;
	}
}
