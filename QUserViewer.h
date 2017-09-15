#ifndef QUSERWIDGET_H
#define QUSERWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Utils.h"
#include "QROSInterface_HSActive.h"
#include "QROSInterface_HSUsers.h"
#include "QROSInterface_ROSInfo.h"
#include "QROSInterface_Torch.h"

class QHS_WidgetItemBase : public QTreeWidgetItem
{
	QString m_id;
protected:
	void setItemData(int column, const QString &visibleText, const QVariant &sortValue, const QString &tip = QString());

public:
	static QStringList UserItemLabels;
	enum Col
	{
		ID = -1,	// Used for searching by id.
		IP = 0,
		MAC,
		Downloaded,	// Total downloaded
		Uploaded,	// Total uploaded
		Download,	// Download speed.
		Upload,		// Upload speed.
		Uptime
	};
	void setID(const QString &id)			{ m_id = id; }
	const QString &id() const				{ return m_id; }

	virtual bool operator<(const QTreeWidgetItem &other) const;
};

class QHSAD_WidgetItem : public QHS_WidgetItemBase
{
public:
	void setIP(const QString &ip)			{ setItemData(IP, ip, Utils::ipToInt(ip), ip); }
	void setMAC(const QString &mac)			{ setText(MAC, mac);}
	void setUserName(const QString &name)	{ setText(IP, name); }
	void setDownloaded(qlonglong ll)		{ setItemData(Downloaded, Utils::readableBits(ll, false), ll, QString("%1").arg(ll)); }
	void setUploaded(qlonglong ll)			{ setItemData(Uploaded, Utils::readableBits(ll, false), ll, QString("%1").arg(ll)); }
	void setDownload(qlonglong ll)			{ setItemData(Download, Utils::readableBits(ll, true), ll, QString("%1").arg(ll)); }
	void setUpload(qlonglong ll)			{ setItemData(Upload, Utils::readableBits(ll, true), ll, QString("%1").arg(ll)); }
	void setUptime(const QString &u)		{ int t = Utils::rosTimeToInt(u); setItemData(Uptime, u, t, QString("%1").arg(t)); }

	void setActiveData(const QROSInterface_HSActive &hsau);
};

class QHSUD_WidgetItem : public QHS_WidgetItemBase
{
public:
	void setUserData(const QROSInterface_HSUsers &hsud);

	int activeCount() const { return childCount(); }
	QHSAD_WidgetItem *activeItem(int index) const;
	QHSAD_WidgetItem *activeItem(const QROSInterface_HSActive &hsad)const;
};

class QUserWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit QUserWidget(QWidget *parent = 0) : QTreeWidget(parent) { }

	void setup();
	int userCount() const;
	QHSUD_WidgetItem *userItem(int pos) const;
	int findUserDataPos(QHSAD_WidgetItem::Col col, const QString &mach) const;
	QHSUD_WidgetItem *findUserDataItem(QHSAD_WidgetItem::Col c, const QString &mach) const;
	QHSUD_WidgetItem *findUserDataItem(const QROSInterface_HSUsers &hsud) const;

	void updateUserData(const QROSInterface_HSUsers &hsud, QHSUD_WidgetItem *item);
	void updateUserData(const QROSInterface_HSUsers &hsud);
	void addUserData(const QROSInterface_HSUsers &hsud);

	int activeCount(const QHSUD_WidgetItem *item) const;
	QHSAD_WidgetItem *activeItem(const QHSUD_WidgetItem *item, int pos) const;
	int findActiveUserPos(QHSAD_WidgetItem::Col col, const QString &mach) const;
	int findActiveUserPos(const Utils::IPv4 &ip);
	QHSAD_WidgetItem *findActiveUserItem(QHSAD_WidgetItem::Col col, const QString &mach) const;
	QHSAD_WidgetItem *findActiveUserItem(const QROSInterface_HSActive &hsau) const;

	void updateActiveUser(const QROSInterface_HSActive &hsau, QHSAD_WidgetItem *item);
	void updateActiveUser(const QROSInterface_HSActive &hsau);
	void addActiveUser(const QROSInterface_HSActive &hsau);
signals:

public slots:
	void onUserDataReceived(const QROSInterface_HSUsers &hsud, bool firstDeploy);
	void onUserDataDeleted(const QROSInterface_HSUsers &hsud);
	void onActiveUserReceived(const QROSInterface_HSActive &hsau, bool firstDeploy);
	void onActiveUserDeleted(const QROSInterface_HSActive &hsau);
	void onTochDataReceived(const TorchInfo &ti);
};

#endif // QUSERWIDGET_H
