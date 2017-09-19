#ifndef QROSINTERFACE_HSUBASE_H
#define QROSINTERFACE_HSUBASE_H

#endif // QROSINTERFACE_HSUBASE_H

#ifndef QROSINTERFACE_HSBASE_H
#define QROSINTERFACE_HSBASE_H

#include "QROSInterface_Base.h"

struct ROSHSUserBase : public ROSInterfaceBaseData
{
	QString m_userName;
	quint64 m_downloadedBits;
	quint64 m_uploadedBits;
	QString m_uptime;
};

/**
 * @brief The QROSInterface_HSUBase class
 * Comod data used by users's data and active's data
 */
class QROSInterface_HSUBase : public QROSInterface_Base
{
Q_OBJECT

protected:
	bool deployingInitialData;
	QList<ROS::QSentence> queuedSentence;

public:
	QROSInterface_HSUBase(ROS::Comm *mktComm, ROSHSUserBase *hotspot_data, QObject *papi)
		: QROSInterface_Base(mktComm, hotspot_data, papi), deployingInitialData(false)
	{ }
};
#endif // QROSINTERFACE_HSBASE_H
