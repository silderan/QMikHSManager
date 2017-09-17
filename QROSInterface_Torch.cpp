#include "QROSInterface_Torch.h"

const QString QROSInterface_Torch::tagTorchTool = QString("X_TorchTool");

QROSInterface_Torch::QROSInterface_Torch(ROS::Comm *mktComm, QObject *papi) : QROSInterface_Base(mktComm, papi)
{
	addValidVersions("0.0.0", "99.99.99");
}

bool QROSInterface_Torch::doRequest()
{
	switch( versionIndex() )
	{
	case 0:
	  {
		ROS::QSentence s("/tool/torch");

		if( !m_interface.isEmpty() )
			s.addAttribute( QString("interface=%1").arg(m_interface) );

		if( ((uint)m_srcAddress) != ((uint)0) )
			s.addAttribute( QString("src-address=%1").arg( m_srcAddress.toString(true, true)) );

		s.addQueries( QStringList() << "type=ether" << "#|" );
		s.setTag(tagTorchTool);
		mktComm()->sendSentence(s);
		return true;
	  }
	}
	return false;
}

void QROSInterface_Torch::fromSentence(const ROS::QSentence &s)
{
	// !re =.section=2=mac-protocol=ip=rx=0=rx-packets=0=src-address=10.4.23.32=tx=0=tx-packets=0.tag=4
	switch( versionIndex() )
	{
	case 0:
	  {
		m_section = s.attribute(".section").toInt();
		m_ip = s.attribute("src-address");
		m_rx = s.attribute("rx").toLongLong();
		m_tx = s.attribute("tx").toLongLong();
		emit dataReceived(*this);
		break;
	  }
	}
}

bool QROSInterface_Torch::parseResponse(const ROS::QSentence &s)
{
	if( s.tag() == tagTorchTool )
	{
		if( s.getResultType() == ROS::QSentence::Reply )
			fromSentence(s);
		return true;
	}
	return false;
}
