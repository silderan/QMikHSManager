#ifndef QROSINTERFACE_ROSINFO_H
#define QROSINTERFACE_ROSINFO_H

#include "QROSInterface_Base.h"

struct ROSInfoData : public ROSInterfaceBaseData
{
	QROSVersion m_rosVersion;
	QString m_identity;
	QString m_boardName;
	QString m_architecture;

	QString toString() const { return QString("%1. %2 %3 (%4)").
				arg(m_identity, m_boardName, m_rosVersion.toString(), m_architecture); }
};

class QROSInterface_ROSInfo : public QROSInterface_Base
{
	Q_OBJECT

protected:
	void clear();

	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);

private:
	const ROSInfoData &rosInfoData() const { return *static_cast<const ROSInfoData*>(rosData()); }
	ROSInfoData &rosInfoData() { return *static_cast<ROSInfoData*>(rosData()); }

	static QList<ROS::QSentence> getROSVersionSentences();

public:
	QROSInterface_ROSInfo(ROS::Comm *mktCom, QObject *papi = Q_NULLPTR);

	QString toString() const { return rosInfoData().toString(); }
signals:
	void versionReceived(const QROSVersion &rv);
	void infoReceived(const ROSInfoData &ri);
	void identityReceived(const QString &id);
};

#endif // QROSINTERFACE_ROSINFO_H
