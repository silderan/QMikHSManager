#ifndef QUSERWIDGET_H
#define QUSERWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDateTime>

#include "Utils.h"
#include "QROSInterface_HSActive.h"
#include "QROSInterface_HSUsers.h"
#include "QROSInterface_ROSInfo.h"
#include "QROSInterface_Torch.h"

class QHS_WidgetItemBase : public QTreeWidgetItem
{
	QString m_id;
	QDateTime m_lastUpdate;
	QString m_userName;

protected:
	void setItemData(int column, const QString &visibleText, const QVariant &sortValue, const QString &tip = QString());
	QString itemText(int column) const { return text(column); }
	QVariant itemValue(int column) const { return data(column, Qt::UserRole); }

public:
	static QStringList UserItemLabels;
	enum Col
	{
		ID = -2,	// Used for searching by id.
		UName = -1,	// Used for searching by UserName.
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

	void setUserName(const QString &uname)	{ m_userName = uname; }
	const QString &userName() const			{ return m_userName; }

	const QDateTime &lastUpdate() const		{ return m_lastUpdate; }
	void setLastUpdate(const QDateTime &d)	{ m_lastUpdate = d; }

	void setDownloadedBits(qlonglong ll)		{ setItemData(Downloaded, Utils::readableBytes(BITS_TO_BYTES(ll), false), ll, QString("%1").arg(ll)); }
	qlonglong downloadedBits() const			{ return itemValue(Downloaded).toLongLong(); }

	void setUploadedBits(qlonglong ll)			{ setItemData(Uploaded, Utils::readableBytes(BITS_TO_BYTES(ll), false), ll, QString("%1").arg(ll)); }
	qlonglong uploadedBits() const				{ return itemValue(Uploaded).toLongLong(); }

	void setUptime(const QString &u)		{ int t = Utils::rosTimeToInt(u); setItemData(Uptime, u, t, QString("%1").arg(t)); }

	void setUserData(const ROSHSUserBase &hsb);
	bool dataMatch(int col, QString match);

	virtual bool operator<(const QTreeWidgetItem &other) const;
};

class QHSUD_WidgetItem;
class QHSAD_WidgetItem : public QHS_WidgetItemBase
{
public:
	~QHSAD_WidgetItem();
	QHSUD_WidgetItem *userDataItem();
	void setIP(const QString &ip)			{ setItemData(IP, ip, Utils::ipToInt(ip), ip); }

	void setMAC(const QString &mac)			{ setText(MAC, mac);}

	void setDownload(qlonglong ll)			{ setItemData(Download, Utils::readableBits(ll, true), ll, QString("%1").arg(ll)); }

	void setUpload(qlonglong ll)			{ setItemData(Upload, Utils::readableBits(ll, true), ll, QString("%1").arg(ll)); }

	void setUserData(const HSActiveUser &hsau);
};

class QHSUD_WidgetItem : public QHS_WidgetItemBase
{
public:
	int activeCount() const { return childCount(); }

	void setUserData(const HSUserData &hsud);

	QHSAD_WidgetItem *activeItem(int index) const;
	QHSAD_WidgetItem *activeItem(const HSActiveUser &hsad)const;
};

class QUserWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit QUserWidget(QWidget *parent = 0) : QTreeWidget(parent) { }

	void setup();
	int userCount() const;
	QHSUD_WidgetItem *userItem(int pos) const;
	int findUserDataPos(QHSAD_WidgetItem::Col col, const QString &match) const;
	QHSUD_WidgetItem *findUserDataItem(QHSAD_WidgetItem::Col c, const QString &mach) const;
	QHSUD_WidgetItem *findUserDataItem(const HSUserData &hsud) const;

	void updateUserData(const HSUserData &hsud, QHSUD_WidgetItem *item);
	void updateUserData(const HSUserData &hsud);
	void addUserData(const HSUserData &hsud);

	int activeCount(const QHSUD_WidgetItem *item) const;
	QHSAD_WidgetItem *activeItem(const QHSUD_WidgetItem *item, int pos) const;
	int findActiveUserPos(const Utils::IPv4 &ip);
	QHSAD_WidgetItem *findActiveUserItem(QHSAD_WidgetItem::Col col, const QString &match) const;
	QHSAD_WidgetItem *findActiveUserItem(const HSActiveUser &hsau) const;

	void updateActiveUser(const HSActiveUser &hsau, QHSAD_WidgetItem *item);
	void updateActiveUser(const HSActiveUser &hsau);
	void addActiveUser(const HSActiveUser &hsau);
signals:

public slots:
	void onUserDataReceived(const HSUserData &hsud, bool firstDeploy);
	void onUserDataDeleted(const HSUserData &hsud);
	void onActiveUserReceived(const HSActiveUser &hsau, bool firstDeploy);
	void onActiveUserDeleted(const HSActiveUser &hsau);
	void onTochDataReceived(const TorchData &td);
};

#endif // QUSERWIDGET_H
