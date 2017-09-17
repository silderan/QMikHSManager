#ifndef QROSINTERFACE_TORCH_H
#define QROSINTERFACE_TORCH_H

#include "QROSInterface_Base.h"
#include "Utils.h"

class QROSInterface_Torch : public QROSInterface_Base
{
	Q_OBJECT

	QString m_interface;
	Utils::IPv4Net m_srcAddress;	// IP range to listen to.

	const static QString tagTorchTool;

	Utils::IPv4 m_ip;
	quint64 m_tx;
	quint64 m_rx;
	int m_section;

protected:
	virtual bool doRequest();
	virtual bool parseResponse(const ROS::QSentence &s);
	void fromSentence(const ROS::QSentence &s);

public:
	QROSInterface_Torch(ROS::Comm *mktComm, QObject *papi);

	void setInterface(const QString &iface) { m_interface = iface; }
	void setSrcAddress(const Utils::IPv4Net &srcAddress) { m_srcAddress = srcAddress; }

	const Utils::IPv4 &targetIP() const { return m_ip; }
	const quint64 &tx() const { return m_tx; }
	const quint64 &rx() const { return m_rx; }
	int section() const { return m_section; }

signals:
	void dataReceived(const QROSInterface_Torch &ti);
};

#endif // QROSINTERFACE_TORCH_H
