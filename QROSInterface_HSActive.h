#ifndef QROSINTERFACE_HSACTIVE_H
#define QROSINTERFACE_HSACTIVE_H

#include "QROSInterface_Base.h"

class QROSInterface_HSActive : public QROSInterface_Base
{
Q_OBJECT

	QString m_userName;
	quint64 m_downloaded;
	quint64 m_uploaded;
	QString m_uptime;
	QString m_IP;
	QString m_MAC;

	const static QString tagGetallData;
	const static QString tagListenData;

	bool deployingInitialData;
	QList<ROS::QSentence> queuedSentence;

public:
	QROSInterface_HSActive(ROS::Comm *mktComm, QObject *papi);

	const QString &userName() const {return m_userName;}
	void setUserName(const QString &userName) {m_userName = userName;}

	quint64 downloaded() const { return m_downloaded; }
	void setDownloaded(const quint64 &down) { m_downloaded = down; }

	quint64 uploaded() const { return m_uploaded; }
	void setUploaded(const quint64 &up) { m_uploaded= up; }

	const QString &uptime() const { return m_uptime; }
	void setUptime(const QString &up) { m_uptime= up; }

	const QString &IP() const {return m_IP;}
	void setIP(const QString &IP) {m_IP = IP;}

	const QString &MAC() const { return m_MAC; }
	void setMAC(const QString &MAC) {m_MAC = MAC;}

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void fromSentence(const ROS::QSentence &s);

signals:
	void dataReceived(const QROSInterface_HSActive &hsud, bool firstDeploy);
	void dataDeleted(const QROSInterface_HSActive &hsud);
};

#endif // QROSINTERFACE_HSACTIVE_H
