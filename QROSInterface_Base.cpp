#include "QROSInterface_Base.h"
#include <QDateTime>

bool QROSVersion::fromString(const QString &s)
{
	QStringList numbers = s.split( QRegExp("\\D+"), QString::SkipEmptyParts );
	m_rv.compactedVersion = 0;
	if( numbers.size() > 0 )
		m_rv.rvG = numbers[0].toInt();
	else
		return false;
	if( numbers.size() > 1 )
		m_rv.rvM = numbers[1].toInt();
	if( numbers.size() > 2 )
		m_rv.rvL = numbers[2].toInt();
	return true;
}

/**
 * @brief QROSVersionChecker::getValidVersionPOS
 * Checks if the ros Version is valid.
 * @param rv the version
 * @return The index of the minimal version valid of the first correct pair or -1 otherwise.
 */
int QROSVersionChecker::getValidVersionPOS(const QROSVersion &rv)
{
	for( int i = 0; i < count(); i+=2 )
	{
		if( (at(i) < rv) && (at(i+1) > rv) )
			return i;
	}
	return -1;
}

QString QROSVersionChecker::toString()
{
	QString rtn = QObject::tr("between");
	for( int i = 0; i < count(); i+=2 )
	{
		if( rtn.length() )
		{
			if( i == (count()-2) )
				rtn += QObject::tr(" or ");
			else
				rtn += QObject::tr(" o ");
		}
		rtn = QObject::tr("%1 and %2").arg(at(i).toString(), at(i+1).toString());
	}
	return rtn;
}

QString QROSInterfaceError::errorString(QROSInterfaceError::ErrorCode e, const QString &errorText)
{
	QString rtn;
	switch(e)
	{
	case ROSVersionUnkown:	rtn = QObject::tr("Cannot obtain ROS Version");
	case ROSVersionMiss:	rtn = QObject::tr("ROS Version missing");
	case ROSVersionLow:		rtn = QObject::tr("ROS Version too low");
	case ROSVersionHight:	rtn = QObject::tr("ROS Version too high");
	case ROSVersionIncompatible:	rtn = QObject::tr("ROS Version not compatible");
	default:				rtn = QObject::tr("Unknown ROS Data Base error");
	}
	if( !errorText.isEmpty() )
		rtn = QString("%1. %2").arg(rtn, errorText);
	return rtn;
}

QROSInterface_Base::QROSInterface_Base(ROS::Comm *mktComm, QObject *papi = Q_NULLPTR) : QObject(papi), m_mktComm(mktComm), m_validVersionIndex(-2)
{
	connect( m_mktComm, &ROS::Comm::comReceive, this, &QROSInterface_Base::onResponce );
}

void QROSInterface_Base::setError(QROSInterfaceError::ErrorCode e, const QString &text)
{
	m_lastError.setError(e, text);
	if( e != QROSInterfaceError::NoEror )
		emit rosInterfaceError(lastError());
}

void QROSInterface_Base::clearError()
{
	m_lastError.clearError();
}

bool QROSInterface_Base::checkROSVersion()
{
	int pos = m_rosVersionChecker.getValidVersionPOS(m_rosVersion);
	if( pos == -1 )
	{
		emit rosInterfaceError( QROSInterfaceError( QROSInterfaceError::ROSVersionLow,
				tr("ROS Version for requesting HotSpot users must be between %1, but %2 is received")
					.arg(m_rosVersionChecker.toString(), m_rosVersion.toString())) );
		return false;
	}
	m_validVersionIndex = pos;
	return true;
}

int QROSInterface_Base::versionIndex() const
{
	Q_ASSERT_X(m_validVersionIndex != -2, "versionIndex()", "Consulting versionIndex, but has never been set");
	return m_validVersionIndex;
}

void QROSInterface_Base::requestData()
{
	qint64 t = QDateTime::currentMSecsSinceEpoch();
	if( doRequest() )
		m_requestTime = t;
}

void QROSInterface_Base::onResponce(const ROS::QSentence &s)
{
	qint64 t = QDateTime::currentMSecsSinceEpoch();
	clearError();
	m_id = s.getID();
	m_responseTime = t;
	parseResponse(s);
}
