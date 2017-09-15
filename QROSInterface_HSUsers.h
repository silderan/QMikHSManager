#ifndef QROSINTERFACE_HSUSERS_H
#define QROSINTERFACE_HSUSERS_H

#include "QROSInterface_Base.h"

class QROSInterface_HSUsers : public QROSInterface_Base
{
Q_OBJECT

	QString m_userName;
	QString m_userPass;
	quint64 m_downloaded;
	quint64 m_uploaded;
	QString m_uptime;

	const static QString tagGetallData;
	const static QString tagListenData;

	bool deployingInitialData;
	QList<ROS::QSentence> queuedSentence;

public:
	QROSInterface_HSUsers(ROS::Comm *mktComm, QObject *papi);

	const QString &userName() const { return m_userName; }
	void setUserName(const QString &uname) { m_userName = uname; }

	const QString &userPass() const { return m_userPass; }
	void setUserPass(const QString &upass) { m_userPass = upass; }

	quint64 downloaded() const { return m_downloaded; }
	void setDownloaded(const quint64 &down) { m_downloaded = down; }

	quint64 uploaded() const { return m_uploaded; }
	void setUploaded(const quint64 &up) { m_uploaded= up; }

	const QString &uptime() const { return m_uptime; }
	void setUptime(const QString &up) { m_uptime= up; }

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	virtual void fromSentence(const ROS::QSentence &s);

signals:
	void dataReceived(const QROSInterface_HSUsers &hsud, bool firstDeploy);
	void allDataReceived();
	void dataDeleted(const QROSInterface_HSUsers &hsud);
};

#endif // QROSINTERFACE_HSUSERS_H
