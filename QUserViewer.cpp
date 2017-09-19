#include "QUserViewer.h"
#include "Utils.h"

QStringList QHS_WidgetItemBase::UserItemLabels = QStringList()
		<< QObject::tr("IP")
		<< QObject::tr("MAC")
		<< QObject::tr("Descargado")
		<< QObject::tr("Cargado")
		<< QObject::tr("Descarga")
		<< QObject::tr("Subida")
		<< QObject::tr("Tiempo Activo");


void QHS_WidgetItemBase::setItemData(int column, const QString &visibleText, const QVariant &sortValue, const QString &tip)
{
	setData(column, Qt::DisplayRole, visibleText);
	setData(column, Qt::UserRole, sortValue);
	setData(column, Qt::ToolTipRole, tip);
}

void QHS_WidgetItemBase::setUserData(const ROSHSUserBase &hsb)
{
	setUserName( hsb.m_userName );
	setID( hsb.m_id );
	setUptime( hsb.m_uptime );
	setUploadedBits( hsb.m_uploadedBits );
	setDownloadedBits( hsb.m_downloadedBits );
}

bool QHS_WidgetItemBase::operator<(const QTreeWidgetItem &other) const
{
	int column = this->treeWidget()->sortColumn();
	QVariant d1 = data(column, Qt::UserRole);
	QVariant d2 = other.data(column, Qt::UserRole);
	if( d1.isValid() && d2.isValid() )
		return d1 < d2;
	return text(column) < other.text(column);
}

bool QHS_WidgetItemBase::dataMatch(int col, QString match)
{
	switch( col )
	{
	case ID: return id() == match;
	case UName: return userName() == match;
	default:
		return text(col) == match;
	}
}

void QHSUD_WidgetItem::setUserData(const HSUserData &hsud)
{
	QHS_WidgetItemBase::setUserData(hsud);

	if( !id().isEmpty() )
	{
		if( hsud.m_userPass.isEmpty() )
			setText( 0, QString("%1").arg(hsud.m_userName) );
		else
			setText( 0, QString("%1/%2").arg(hsud.m_userName, hsud.m_userPass) );
	}
	else
	{
		if( hsud.m_userPass.isEmpty() )
			setText( 0, QObject::tr("[Deleted user] %1").arg(hsud.m_userName) );
		else
			setText( 0, QObject::tr("[Deleted user] %1/%2").arg(hsud.m_userName, hsud.m_userPass) );
	}
}

QHSAD_WidgetItem *QHSUD_WidgetItem::activeItem(int index) const
{
	return dynamic_cast<QHSAD_WidgetItem*>(child(index));
}


QHSAD_WidgetItem::~QHSAD_WidgetItem()
{
	// User data item has no ID when it has been deleted but there is
	// active users data.
	// So, when the last one active user is deleted, we must delete "parent" user data
	if( userDataItem() && userDataItem()->id().isEmpty() )
		delete userDataItem();
}

QHSUD_WidgetItem *QHSAD_WidgetItem::userDataItem()
{
	return dynamic_cast<QHSUD_WidgetItem*>(parent());
}

void QHSAD_WidgetItem::setUserData(const HSActiveUser &hsau)
{
	QHS_WidgetItemBase::setUserData(hsau);
	setIP( hsau.m_IP );
	setMAC( hsau.m_MAC );
}

void QUserWidget::setup()
{
	setColumnCount( QHSUD_WidgetItem::UserItemLabels.count() );
	setHeaderLabels( QHSUD_WidgetItem::UserItemLabels );
	setSortingEnabled(true);
}

int QUserWidget::userCount() const
{
	return topLevelItemCount();
}

QHSAD_WidgetItem *QHSUD_WidgetItem::activeItem(const HSActiveUser &hsad) const
{
	for( int i = 0; i < activeCount(); i++ )
		if( activeItem(i)->id() == hsad.m_id )
			return activeItem(i);

	return Q_NULLPTR;
}

QHSUD_WidgetItem *QUserWidget::userItem(int pos) const
{
	return dynamic_cast<QHSUD_WidgetItem*>(topLevelItem(pos));
}

int QUserWidget::findUserDataPos(QHSUD_WidgetItem::Col col, const QString &match) const
{
	for( int i = userCount()-1; i >= 0; i-- )
	{
		if( userItem(i)->dataMatch(col, match) )
			return i;
	}
	return -1;
}

QHSUD_WidgetItem *QUserWidget::findUserDataItem(QHSUD_WidgetItem::Col c, const QString &mach) const
{
	int pos = findUserDataPos(c, mach);
	return (pos == -1) ? Q_NULLPTR : userItem(pos);
}

/**
 * @brief QUserWidget::findUserDataItem
 * Try to find the HotSpot user data using the ID and the username.
 * This funcion uses QUserDataWidgetItem *QUserWidget::findUserDataItem(QUserDataWidgetItem::Col c, const QString &mach) const
 * @param hsud the HotSpotUserData class
 * @return the TopLevelItem or null
 */
QHSUD_WidgetItem *QUserWidget::findUserDataItem(const HSUserData &hsud) const
{
	QHSUD_WidgetItem *item;
	if( !hsud.m_id.isEmpty() && ((item = findUserDataItem(QHSUD_WidgetItem::ID, hsud.m_id)) != Q_NULLPTR) )
		return item;
	if( !hsud.m_userName.isEmpty() && ((item = findUserDataItem(QHSUD_WidgetItem::UName, hsud.m_userName)) != Q_NULLPTR) )
		return item;
	return Q_NULLPTR;
}

void QUserWidget::updateUserData(const HSUserData &hsud, QHSUD_WidgetItem *item)
{
	item->setUserData(hsud);
}

void QUserWidget::updateUserData(const HSUserData &hsud)
{
	updateUserData(hsud, findUserDataItem( QHSUD_WidgetItem::ID, hsud.m_id) );
}

void QUserWidget::addUserData(const HSUserData &hsud)
{
	QHSUD_WidgetItem *item = new QHSUD_WidgetItem();
	addTopLevelItem(item);
	updateUserData(hsud, item);
}

int QUserWidget::activeCount(const QHSUD_WidgetItem *item) const
{
	return item->childCount();
}

QHSAD_WidgetItem *QUserWidget::activeItem(const QHSUD_WidgetItem *item, int pos) const
{
	return dynamic_cast<QHSAD_WidgetItem *>(item->child(pos));
}

QHSAD_WidgetItem *QUserWidget::findActiveUserItem(QHSUD_WidgetItem::Col col, const QString &match) const
{
	for( int t = 0; t < userCount(); t++ )
	{
		for( int a = 0; a < userItem(t)->activeCount(); a++ )
			if( userItem(t)->activeItem(a)->dataMatch(col, match) )
				return userItem(t)->activeItem(a);
	}
	return Q_NULLPTR;
}

QHSAD_WidgetItem *QUserWidget::findActiveUserItem(const HSActiveUser &hsau) const
{
	QHSUD_WidgetItem *udItem = findUserDataItem(QHSAD_WidgetItem::UName, hsau.m_userName);
	if( udItem )
		return udItem->activeItem(hsau);
	return Q_NULLPTR;
}

void QUserWidget::updateActiveUser(const HSActiveUser &hsau, QHSAD_WidgetItem *item)
{
	item->setUserData(hsau);
}

void QUserWidget::updateActiveUser(const HSActiveUser &hsau)
{

}

void QUserWidget::addActiveUser(const HSActiveUser &hsau)
{
	QHSUD_WidgetItem *item = findUserDataItem(QHSAD_WidgetItem::UName, hsau.m_userName);

	if( !item )
	{
		// If there isn't user data, could be becouse the data has been deleted.
		// But the active users are still conected.
		// So, let's create a dummy user data.
		// TODO:
		return;
	}

	QHSAD_WidgetItem *au_item = new QHSAD_WidgetItem();
	item->addChild(au_item);
	updateActiveUser(hsau, au_item);
}

void QUserWidget::onUserDataReceived(const HSUserData &hsud, bool firstDeploy)
{
	if( firstDeploy )
		addUserData(hsud);
	else
	{
		QHSUD_WidgetItem *item = findUserDataItem(hsud);
		if( item != Q_NULLPTR )
			updateUserData(hsud,item);
		else
			addUserData(hsud);
	}
}

void QUserWidget::onUserDataDeleted(const HSUserData &hsud)
{

}

void QUserWidget::onActiveUserReceived(const HSActiveUser &hsau, bool firstDeploy)
{
	if( firstDeploy )
		addActiveUser(hsau);
	else
	{
		QHSAD_WidgetItem *item = findActiveUserItem(hsau);
		if( item )
			updateActiveUser(hsau, item);
		else
			addActiveUser(hsau);
	}
}

void QUserWidget::onActiveUserDeleted(const HSActiveUser &hsau)
{
	// In this case, ROS only give us the ID.
	QHSAD_WidgetItem *item = findActiveUserItem( QHS_WidgetItemBase::ID, hsau.m_id );
	if( item )
		delete item;
}

void QUserWidget::onTochDataReceived(const TorchData &td)
{
	// TODO: This MUST be improved to speed up this process.
	// I thougth this could be done by collecting all section data without updating view
	// and once all have been updated, loop through all tree items to update his data.
	// Even the data collecter cound be stored in a static and grow-only hash-table/tree.

	QHSUD_WidgetItem *udItem;
	QHSAD_WidgetItem *auItem;
	uint ip;
	for( int t = topLevelItemCount()-1; t>=0; t-- )
	{
		udItem = userItem(t);
		for( int i = udItem->childCount()-1; i>=0; i-- )
		{
			auItem = udItem->activeItem(i);
			ip = auItem->data(QHS_WidgetItemBase::IP, Qt::UserRole).toUInt();
			if( ip == (uint) td.m_ip )
			{
				auItem->setUpload(td.m_rx);
				auItem->setUploadedBits(auItem->uploadedBits()+td.m_rx);
				auItem->setDownload(td.m_tx);
				auItem->setDownloadedBits(auItem->downloadedBits()+td.m_rx);
				auItem->setLastUpdate(QDateTime::currentDateTime());
			}
		}
	}
}
