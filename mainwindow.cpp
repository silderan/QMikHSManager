#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QStandardPaths>

#include <QMessageBox>

#define TAG_USER		"User"		// Receiving current secret data.
#define TAG_LUSER		"LUser"		// Listening secret data changes.
#define TAG_PROFILE		"Perfil"	// Receiving current profile data.
#define TAG_LPROFILE	"LProfile"	// Listening profile data changes.
#define TAG_ACTIVE		"Active"	// Receiving current active data.
#define TAG_LACTIVE		"LActive"	// Listening active data changes.
#define TAG_NEW			"Nuevo"		// Tag used just to send new secret.

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_rosInfo(&mktAPI, this),
	m_hsInfo(&mktAPI, this),
	m_hsUsers(&mktAPI, this),
	m_hsActive(&mktAPI, this),
	m_torch(&mktAPI, this)
{
	ui->setupUi(this);
	ui->twUsers->setup();

	QStringList allPaths = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	Q_ASSERT(!allPaths.isEmpty());
	QString path = allPaths.first();
	Q_ASSERT(!path.isEmpty());
	QString fname = QString("%1/%2.ini").arg(path, qApp->applicationName());
	globalConfig.load(fname);

	menuWrapper.deployToolBar(ui->toolBar);
	connect( &menuWrapper, SIGNAL(onConfigLoginClicked(QConfigData::ID)), this, SLOT(configLogin(QConfigData::ID)) );
	connect( &menuWrapper, SIGNAL(onDelRouterClicked(QConfigData::ID)), this, SLOT(removeRouter(QConfigData::ID)) );
	connect( &menuWrapper, SIGNAL(onToggleConnClicked(QConfigData::ID)), this, SLOT(toggleConn(QConfigData::ID)) );

	connect( &mktAPI, SIGNAL(comError(ROS::Comm::CommError,QAbstractSocket::SocketError)), this, SLOT(onComError(ROS::Comm::CommError,QAbstractSocket::SocketError)) );

	connect( &mktAPI, SIGNAL(comStateChanged(ROS::Comm::CommState)), this, SLOT(onStateChanged(ROS::Comm::CommState)) );
	connect( &mktAPI, SIGNAL(comStateChanged(ROS::Comm::CommState)), &menuWrapper, SLOT(onStateChanged(ROS::Comm::CommState)) );

	connect( &mktAPI, SIGNAL(loginStateChanged(ROS::Comm::LoginState)), this, SLOT(onLoginChanged(ROS::Comm::LoginState)) );
	connect( &mktAPI, SIGNAL(loginStateChanged(ROS::Comm::LoginState)), &menuWrapper, SLOT(onLoginChanged(ROS::Comm::LoginState)) );

	connect( &m_rosInfo, &QROSInterface_ROSInfo::versionReceived, this, &MainWindow::onROSVersionReceived );
	connect( &m_rosInfo, &QROSInterface_ROSInfo::infoReceived, this, &MainWindow::onROSInfoReceived );
	connect( &m_rosInfo, &QROSInterface_ROSInfo::rosInterfaceError, this, &MainWindow::onInterfaceError );
	connect( &m_rosInfo, &QROSInterface_ROSInfo::identityReceived, this, &MainWindow::onROSIdentityReceived );

	connect( &m_hsInfo, SIGNAL(infoReceived(QROSInterface_HSInfo)), this, SLOT(onHSInfoReceived(QROSInterface_HSInfo)) );
	connect( &m_hsInfo, SIGNAL(profileReceived(QROSInterface_HSInfo)), this, SLOT(onHSProfileReceived(QROSInterface_HSInfo)) );
	connect( &m_hsInfo, SIGNAL(allDataReceived(QROSInterface_HSInfo)), this, SLOT(onHSAllInfoReceived(QROSInterface_HSInfo)) );
	connect( &m_hsInfo, SIGNAL(rosInterfaceError(QROSInterfaceError)), this, SLOT(onInterfaceError(QROSInterfaceError)) );

	connect( &m_hsUsers, SIGNAL(dataReceived(QROSInterface_HSUsers,bool)), ui->twUsers, SLOT(onUserDataReceived(QROSInterface_HSUsers,bool)) );
	connect( &m_hsUsers, SIGNAL(dataDeleted(QROSInterface_HSUsers)), ui->twUsers, SLOT(onUserDataDeleted(QROSInterface_HSUsers)) );
	connect( &m_hsUsers, SIGNAL(allDataReceived()), this, SLOT(onAllUserDataReceived()) );
	connect( &m_hsUsers, SIGNAL(rosInterfaceError(QROSInterfaceError)), this, SLOT(onInterfaceError(QROSInterfaceError)) );

	connect( &m_hsActive, SIGNAL(dataReceived(QROSInterface_HSActive,bool)), ui->twUsers, SLOT(onActiveUserReceived(QROSInterface_HSActive,bool)) );
	connect( &m_hsActive, SIGNAL(dataDeleted(QROSInterface_HSActive)), ui->twUsers, SLOT(onActiveUserDeleted(QROSInterface_HSActive)) );
	connect( &m_hsActive, SIGNAL(rosInterfaceError(QROSInterfaceError)), this, SLOT(onInterfaceError(QROSInterfaceError)) );

	connect( &m_torch, SIGNAL(dataReceived(TorchInfo)), ui->twUsers, SLOT(onTochDataReceived(TorchInfo)) );
	connect( &m_torch, SIGNAL(rosInterfaceError(QROSInterfaceError)), this, SLOT(onInterfaceError(QROSInterfaceError)) );
}

MainWindow::~MainWindow()
{
	menuWrapper.disconnect();
	mktAPI.disconnect();
	delete ui;
	ui = NULL;
}

void MainWindow::setStatusText(const QString &txt)
{
	if( ui )
		ui->statusBar->showMessage(txt);
}

void MainWindow::removeRouter(QConfigData::ID id)
{
	globalConfig.removeRouterData(id);
	menuWrapper.onRouterDeleted(id);
}

void MainWindow::toggleConn(QConfigData::ID id)
{
	// If it is currently closing, force it.
	if( mktAPI.isClosing() )
		mktAPI.closeCom(true);
	else
	// If already loged in, connected or connecting, close connection gracefully.
	if( mktAPI.isLoged() || mktAPI.isConnected() || mktAPI.isConnecting() )
		mktAPI.closeCom();
	else
	{
		activeRouterID = id; // Maybe, it is useless.
		mktAPI.setRemoteHost( globalConfig.getHost(id), globalConfig.getPort(id) );
		mktAPI.setUserNamePass( globalConfig.getUserName(id), globalConfig.getUserPass(id) );
		mktAPI.connectToROS();
	}
}

#include "DlgRouterConfig.h"
void MainWindow::configLogin(QConfigData::ID id)
{
	QConfigRouterData routerData;
	routerData.id = id;
	routerData.name = globalConfig.getName(routerData.id);
	routerData.IP = globalConfig.getHost(routerData.id);
	routerData.port = globalConfig.getPort(routerData.id);
	routerData.uname = globalConfig.getUserName(routerData.id);
	routerData.upass = globalConfig.getUserPass(routerData.id);

	DlgRouterConfig *dlg = new DlgRouterConfig(routerData, this);
	if( dlg->exec() == QDialog::Accepted )
	{
		globalConfig.setName(routerData.id, routerData.name);
		globalConfig.setHost(routerData.id, routerData.IP);
		globalConfig.setPort(routerData.id, routerData.port);
		globalConfig.setUserName(routerData.id, routerData.uname);
		globalConfig.setUserPass(routerData.id, routerData.upass);
		menuWrapper.updateToolBar(id);
	}
}

void MainWindow::onComError(ROS::Comm::CommError, QAbstractSocket::SocketError)
{
	QString s = mktAPI.errorString();
	setStatusText(s);
	QMessageBox::warning(this, objectName(), tr("Error reportado por la red, router o sistema:\n%1").arg(s));
}

void MainWindow::onInterfaceError(const QROSInterfaceError &err)
{
	QString s = err.errorText();
	setStatusText(s);
	QMessageBox::warning(this, objectName(), tr("ROS Interface error:\n%1").arg(s));
}

void MainWindow::onROSInfoReceived(const QROSInterface_ROSInfo &rosInfo)
{
	setWindowTitle(rosInfo.toString());
}

void MainWindow::onStateChanged(ROS::Comm::CommState s)
{
	menuWrapper.onStateChanged(s);
	switch( s )
	{
	case ROS::Comm::Unconnected:
		setStatusText( tr("Desconectado") );
		break;
	case ROS::Comm::HostLookup:
		setStatusText( tr("Resolviendo URL") );
		break;
	case ROS::Comm::Connecting:
		setStatusText( tr("Conectando al servidor") );
		break;
	case ROS::Comm::Connected:
		setStatusText( tr("Conectado") );
		break;
	case ROS::Comm::Closing:
		setStatusText( tr("Cerrando conexión") );
		break;
	}
}

void MainWindow::onLoginChanged(ROS::Comm::LoginState s)
{
	switch( s )
	{
	case ROS::Comm::NoLoged:
		setStatusText( tr("No está identificado en el servidor") );
		ui->twUsers->setEnabled(false);
		break;
	case ROS::Comm::LoginRequested:
		setStatusText( tr("Pidiendo acceso al router") );
		ui->twUsers->setEnabled(false);
		break;
	case ROS::Comm::UserPassSended:
		setStatusText( tr("Usuario y contraseña enviados") );
		ui->twUsers->setEnabled(false);
		break;
	case ROS::Comm::LogedIn:
		setStatusText( tr("Logado al servidor") );
		ui->twUsers->clear();
		ui->twUsers->setEnabled(true);
		m_rosInfo.requestData();
		break;
	}
}

void MainWindow::onROSVersionReceived(const QROSVersion &rv)
{
	setWindowTitle(m_rosInfo.toString());
	// TODO: check ROS Version on all other data classes and request data
	if( !m_hsUsers.setROSVersion(rv) ||
		!m_hsActive.setROSVersion(rv) ||
		!m_hsInfo.setROSVersion(rv) ||
		!m_torch.setROSVersion(rv) )
		mktAPI.closeCom();
	else
	{
		m_hsUsers.requestData();
		m_hsInfo.requestData();
	}
}

void MainWindow::onROSIdentityReceived(const QString &id)
{
	Q_UNUSED(id);
	setWindowTitle(m_rosInfo.toString());
}

void MainWindow::onHSInfoReceived(const QROSInterface_HSInfo &hsinfo)
{

}

void MainWindow::onHSProfileReceived(const QROSInterface_HSInfo &hsinfo)
{

}

void MainWindow::onHSAllInfoReceived(const QROSInterface_HSInfo &hsinfo)
{
	m_torch.setInterface(hsinfo.interface());
	m_torch.setSrcAddress(hsinfo.hotspotNetwork());
	m_torch.requestData();
}

void MainWindow::onAllUserDataReceived()
{
	m_hsActive.requestData();
}
