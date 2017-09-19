#ifndef QROSINTERFACE_HSUSERS_H
#define QROSINTERFACE_HSUSERS_H

#include "QROSInterface_HSUBase.h"

struct HSUserData : public ROSHSUserBase
{
	QString m_userPass;
};

class QROSInterface_HSUsers : public QROSInterface_HSUBase
{
Q_OBJECT

	HSUserData hsud;
	const static QString tagGetallData;
	const static QString tagListenData;

public:
	QROSInterface_HSUsers(ROS::Comm *mktComm, QObject *papi);

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	virtual void fromSentence(const ROS::QSentence &s);

signals:
	void dataReceived(const HSUserData &hsud, bool firstDeploy);
	void allDataReceived(const HSUserData &hsud);
	void dataDeleted(const HSUserData &hsud);
};

#endif // QROSINTERFACE_HSUSERS_H
