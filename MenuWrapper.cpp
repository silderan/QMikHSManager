#include "MenuWrapper.h"

MenuWrapper::MenuWrapper(QObject *parent) : QObject(parent)
{
}

MenuWrapper::~MenuWrapper()
{

}

void MenuWrapper::addNewRouter(const QConfigData::ID &id)
{
	int pos = cbRouterIDs->count();
	cbRouterIDs->addItem(globalConfig.getName(id));
	cbRouterIDs->setItemData(pos, id);
	updateToolBar(id);
}

void MenuWrapper::deployToolBar(QToolBar *toolBar)
{
	m_toolBar = toolBar;

	m_toolBar->addWidget(cbRouterIDs = new QComboBox(m_toolBar));
	m_toolBar->addWidget(pbConnect = new QPushButton(tr("Conectar"), m_toolBar));
	connect(pbConnect, SIGNAL(clicked()), this, SLOT(onToggleConnClicked()));

	m_toolBar->addWidget(tbAdd = new QToolButton(m_toolBar));
	tbAdd->setText("+");
	connect(tbAdd, SIGNAL(clicked()), this, SLOT(onAddRouterClicked()));

	m_toolBar->addWidget(tbDel = new QToolButton(m_toolBar));
	tbDel->setText("-");
	connect(tbDel, SIGNAL(clicked()), this, SLOT(onDelRouterClicked()));

	m_toolBar->addWidget(tbCnf = new QToolButton(m_toolBar));
	tbCnf->setText("*");
	connect(tbCnf, SIGNAL(clicked()), this, SLOT(onConfigLoginClicked()));

	foreach( QConfigData::ID id, globalConfig.getIDs() )
		addNewRouter(id);
}

int MenuWrapper::findCBItem(const QConfigData::ID &id)
{
	for( int i = cbRouterIDs->count()-1; i >= 0; i-- )
	{
		if( cbRouterIDs->itemData(i).toString() == id )
			return i;
	}
	return -1;
}

void MenuWrapper::onToggleConnClicked()
{
	emit onToggleConnClicked(cbRouterIDs->currentData().toString());
}

void MenuWrapper::onDelRouterClicked()
{
	emit onDelRouterClicked(cbRouterIDs->currentData().toString());
}

void MenuWrapper::onAddRouterClicked()
{
	emit onConfigLoginClicked(globalConfig.firstUnusedID());
}

void MenuWrapper::onConfigLoginClicked()
{
	emit onConfigLoginClicked(cbRouterIDs->currentData().toString());
}

void MenuWrapper::updateToolBar(const QConfigData::ID &id)
{
	int pos = findCBItem(id);
	if( pos == -1 )
		addNewRouter(id);
	else
		cbRouterIDs->setItemText(pos, globalConfig.getName(id));
}

void MenuWrapper::onRouterDeleted(const QConfigData::ID &id)
{
	cbRouterIDs->removeItem(findCBItem(id));
}

void MenuWrapper::onStateChanged(ROS::Comm::CommState s)
{
	switch( s )
	{
	case ROS::Comm::Unconnected:
		pbConnect->setText( tr("Conectar") );
		cbRouterIDs->setEnabled(true);
		break;
	case ROS::Comm::HostLookup:
		pbConnect->setText( tr("Cancelar") ) ;
		cbRouterIDs->setEnabled(false);
		break;
	case ROS::Comm::Connecting:
		cbRouterIDs->setEnabled(false);
		pbConnect->setText( tr("Cancelar") );
		break;
	case ROS::Comm::Connected:
		cbRouterIDs->setEnabled(false);
		pbConnect->setText( tr("Desconectar") );
		break;
	case ROS::Comm::Closing:
		cbRouterIDs->setEnabled(false);
		pbConnect->setText( tr("Forzar desconexión") );
		break;
	}
}

void MenuWrapper::onLoginChanged(ROS::Comm::LoginState s)
{

}
