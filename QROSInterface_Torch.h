#ifndef QROSINTERFACE_TORCH_H
#define QROSINTERFACE_TORCH_H

#include "QROSInterface_Base.h"
#include "Utils.h"

struct TorchInfo
{
	Utils::IPv4 ip;
	quint64 tx;
	quint64 rx;
	int section;
};

class QROSInterface_Torch : public QROSInterface_Base
{
	Q_OBJECT

	QString m_interface;

	Utils::IPv4Net m_srcAddress;	// IP range to listen to.

	const static QString tagTorchTool;

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void fromSentence(const ROS::QSentence &s);

public:
	QROSInterface_Torch(ROS::Comm *mktComm, QObject *papi);

	void setInterface(const QString &iface) { m_interface = iface; }
	void setSrcAddress(const Utils::IPv4Net &srcAddress) { m_srcAddress = srcAddress; }

signals:
	void dataReceived(const TorchInfo &ti);
};

#endif // QROSINTERFACE_TORCH_H
