#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "MenuWrapper.h"
#include "QROSInterface_ROSInfo.h"
#include "QROSInterface_HSInfo.h"
#include "QROSInterface_HSActive.h"
#include "QROSInterface_HSUsers.h"
#include "QROSInterface_Torch.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow *ui;

	QConfigData::ID activeRouterID;
	MenuWrapper menuWrapper;
	ROS::Comm mktAPI;
	QROSInterface_ROSInfo m_rosInfo;
	QROSInterface_HSInfo m_hsInfo;
	QROSInterface_HSUsers m_hsUsers;
	QROSInterface_HSActive m_hsActive;
	QROSInterface_Torch m_torch;

	void setStatusText(const QString &txt);

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	// Menu wrapper slots
	void removeRouter(QConfigData::ID id);
	void toggleConn(QConfigData::ID id);
	void configLogin(QConfigData::ID id);

	// Mikrotik Comm slots
	void onComError(ROS::Comm::CommError, QAbstractSocket::SocketError);
	void onInterfaceError(const QROSInterfaceError &err);
	void onROSInfoReceived(const QROSInterface_ROSInfo &rosInfo);
	void onStateChanged(ROS::Comm::CommState s);
	void onLoginChanged(ROS::Comm::LoginState s);

	void onHSInfoReceived(const QROSInterface_HSInfo &hsinfo);
	void onHSProfileReceived(const QROSInterface_HSInfo &hsinfo);
	void onHSAllInfoReceived(const QROSInterface_HSInfo &hsinfo);

	void onROSVersionReceived(const QROSVersion &rv);
	void onROSIdentityReceived(const QString &id);

	void onAllUserDataReceived();
};

#endif // MAINWINDOW_H
