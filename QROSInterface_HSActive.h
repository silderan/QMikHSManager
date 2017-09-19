#ifndef QROSINTERFACE_HSACTIVE_H
#define QROSINTERFACE_HSACTIVE_H

#include "QROSInterface_HSUBase.h"

struct HSActiveUser : public ROSHSUserBase
{
	QString m_IP;
	QString m_MAC;
};

class QROSInterface_HSActive : public QROSInterface_HSUBase
{
Q_OBJECT

	const static QString tagGetallData;
	const static QString tagListenData;

public:
	QROSInterface_HSActive(ROS::Comm *mktComm, QObject *papi);

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void fromSentence(const ROS::QSentence &s);

signals:
	void dataReceived(const HSActiveUser &hsud, bool firstDeploy);
	void dataDeleted(const HSActiveUser &hsud);
};

#endif // QROSINTERFACE_HSACTIVE_H
