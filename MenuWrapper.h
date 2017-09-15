#ifndef MENUWRAPPER_H
#define MENUWRAPPER_H

/*
 This class just encapsules the menu behaviour.
 */

#include <QToolBar>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

#include "qconfigdata.h"
#include "Comm.h"

class MenuWrapper : public QObject
{
	Q_OBJECT
	QToolBar *m_toolBar;
	QComboBox *cbRouterIDs;
	QPushButton *pbConnect;
	QToolButton *tbAdd;
	QToolButton *tbDel;
	QToolButton *tbCnf;

public:
	explicit MenuWrapper(QObject *parent = 0);
	~MenuWrapper();

	int findCBItem(const QConfigData::ID &id);
	void deployToolBar(QToolBar *toolBar);
	void addNewRouter(const QConfigData::ID &id);

private slots:
	void onAddRouterClicked();
	void onDelRouterClicked();
	void onToggleConnClicked();
	void onConfigLoginClicked();

signals:
	void onDelRouterClicked(const QConfigData::ID &id);
	void onToggleConnClicked(const QConfigData::ID &id);
	void onConfigLoginClicked(const QConfigData::ID &id);

public slots:
	void updateToolBar(const QConfigData::ID &id);
	void onRouterDeleted(const QConfigData::ID &id);
	void onStateChanged(ROS::Comm::CommState s);
	void onLoginChanged(ROS::Comm::LoginState s);
};

#endif // MENUWRAPPER_H
