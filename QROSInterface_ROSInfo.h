#ifndef QROSINTERFACE_ROSINFO_H
#define QROSINTERFACE_ROSINFO_H

#include "QROSInterface_Base.h"

class QROSInterface_ROSInfo : public QROSInterface_Base
{
	Q_OBJECT

	QROSVersion m_rosVersion;
	QString m_identity;
	QString m_boardName;
	QString m_architecture;

protected:
	void clear();

	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);

public:
	QROSInterface_ROSInfo(ROS::Comm *mktCom, QObject *papi = Q_NULLPTR) : QROSInterface_Base(mktCom, papi) { }

	const QROSVersion &rosVersion() const { return m_rosVersion; }
	void setRosVersion(const QROSVersion &rv) { m_rosVersion = rv; }

	static QList<ROS::QSentence> getROSVersionSentences();
	bool fromROSSentence( const ROS::QSentence &s );

	QString toString() const { return QString("%1. %2 %3 (%4)").arg(m_identity, m_boardName, m_rosVersion.toString(), m_architecture); }

signals:
	void versionReceived(const QROSVersion &rv);
	void infoReceived(const QROSInterface_ROSInfo &ri);
	void identityReceived(const QString &id);
};

#endif // QROSINTERFACE_ROSINFO_H
