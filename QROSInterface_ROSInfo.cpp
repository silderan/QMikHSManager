#include "QROSInterface_ROSInfo.h"

/**
 * @brief QROSVersion::getROSVersionSentence
 * Use it to get all known sentences to retreive the ROS version from router.
 * Every sentence in list will have the tag passed plus a ".<number>" Use
 * this number to know the current sentence requested to retrieve the next
 * one in case it doesn't suit on your ROS version.
 * @param tag
 * @see isValid
 * @return A list of ROS::QSentence
 */
QList<ROS::QSentence> QROSInterface_ROSInfo::getROSVersionSentences()
{
	ROS::QSentence s1("/system/resource/getall", "ROSVersion.1");
	return QList<ROS::QSentence>() << s1;
}

void QROSInterface_ROSInfo::clear()
{
	m_rosVersion.clear();
	m_architecture.clear();
	m_boardName.clear();
	m_lastError.clearError();
	m_identity.clear();
}

bool QROSInterface_ROSInfo::doRequest()
{
	clear();
	mktComm()->sendSentence( ROS::QSentence("/system/identity/getall", "ROSIdentity") );
	mktComm()->sendSentence( getROSVersionSentences().first() );
	return true;
}

bool QROSInterface_ROSInfo::parseResponse(const ROS::QSentence &s)
{
	if( s.tag().startsWith("ROSVersion") )
	{
		switch( s.getResultType() )
		{
		case ROS::QSentence::Reply:
			if( m_rosVersion.fromString(s.attribute("version")) )
			{
				m_architecture = s.attribute("architecture-name");
				m_boardName = s.attribute("board-name");
			}
			break;
		case ROS::QSentence::Done:
		default:
			if( m_rosVersion.isValid() )
			{
				emit versionReceived(rosVersion());
				if( !m_identity.isEmpty() )
					emit infoReceived(*this);
				break;
			}
			else
			{
				int number = s.tag().right(1).toInt();
				QList<ROS::QSentence> sentences = getROSVersionSentences();
				if( number < sentences.count() )
				{
					mktComm()->sendSentence(sentences[number]);
					break;
				}
			}
			setError(QROSInterfaceError::ROSVersionUnkown, QObject::tr("Cannot obtain router info"));
			break;
		}
		return true;
	}
	else
	if( s.tag() == "ROSIdentity" )
	{
		switch( s.getResultType() )
		{
		case ROS::QSentence::Done:
			emit identityReceived(m_identity);
			if( m_rosVersion.isValid() )
				emit infoReceived(*this);
			break;
		case ROS::QSentence::Reply:
			m_identity = s.attribute("name");
			if( m_identity.isEmpty() )
				m_identity = "<No name>";
			break;
		default:
			setError(QROSInterfaceError::ROSVersionUnkown, QObject::tr("Cannot obtain router identity"));
			break;
		}
		return true;
	}
	return false;
}
