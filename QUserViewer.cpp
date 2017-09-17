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

bool QHS_WidgetItemBase::operator<(const QTreeWidgetItem &other) const
{
	int column = this->treeWidget()->sortColumn();
	QVariant d1 = data(column, Qt::UserRole);
	QVariant d2 = other.data(column, Qt::UserRole);
	if( d1.isValid() && d2.isValid() )
		return d1 < d2;
	return text(column) < other.text(column);
}

void QHSUD_WidgetItem::setUserData(const QROSInterface_HSUsers &hsud)
{
	setID( hsud.id() );
	setText( 0, hsud.userName() );
}

QHSAD_WidgetItem *QHSUD_WidgetItem::activeItem(int index) const
{
	return dynamic_cast<QHSAD_WidgetItem*>(child(index));
}


void QHSAD_WidgetItem::setActiveData(const QROSInterface_HSActive &hsau)
{
	setID(hsau.id());
	setIP(hsau.IP());
	setMAC(hsau.MAC());
	setUptime(hsau.uptime());
	setUploaded(hsau.uploaded()<<4);
	setDownloaded(hsau.downloaded()<<4);
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

QHSAD_WidgetItem *QHSUD_WidgetItem::activeItem(const QROSInterface_HSActive &hsad) const
{
	for( int i = 0; i < activeCount(); i++ )
		if( activeItem(i)->id() == hsad.id() )
			return activeItem(i);

	return Q_NULLPTR;
}

QHSUD_WidgetItem *QUserWidget::userItem(int pos) const
{
	return dynamic_cast<QHSUD_WidgetItem*>(topLevelItem(pos));
}

int QUserWidget::findUserDataPos(QHSUD_WidgetItem::Col col, const QString &mach) const
{
	for( int i = userCount()-1; i >= 0; i-- )
	{
		if( col == QHS_WidgetItemBase::ID )
		{
			if( userItem(i)->id() == mach )
				return i;
		}
		else
			if( userItem(i)->text(col) == mach )
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
QHSUD_WidgetItem *QUserWidget::findUserDataItem(const QROSInterface_HSUsers &hsud) const
{
	QHSUD_WidgetItem *item;
	if( !hsud.id().isEmpty() && ((item = findUserDataItem(QHSUD_WidgetItem::ID, hsud.id())) != Q_NULLPTR) )
		return item;
	if( !hsud.userName().isEmpty() && ((item = findUserDataItem(QHSUD_WidgetItem::IP, hsud.userName())) != Q_NULLPTR) )
		return item;
	return Q_NULLPTR;
}

void QUserWidget::updateUserData(const QROSInterface_HSUsers &hsud, QHSUD_WidgetItem *item)
{
	item->setUserData(hsud);
}

void QUserWidget::updateUserData(const QROSInterface_HSUsers &hsud)
{
	updateUserData(hsud, findUserDataItem(QHSUD_WidgetItem::ID, hsud.id()));
}

void QUserWidget::addUserData(const QROSInterface_HSUsers &hsud)
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
		{
			if( col == QHSAD_WidgetItem::ID )
			{
				if( userItem(t)->activeItem(a)->id() == match )
					return userItem(t)->activeItem(a);
			}
			else
			{
				if( userItem(t)->activeItem(a)->text(col) == match )
					return userItem(t)->activeItem(a);
			}
		}
	}
	return Q_NULLPTR;
}

QHSAD_WidgetItem *QUserWidget::findActiveUserItem(const QROSInterface_HSActive &hsau) const
{
	QHSUD_WidgetItem *udItem = findUserDataItem(QHSAD_WidgetItem::IP, hsau.userName());
	if( udItem )
		return udItem->activeItem(hsau);
	return Q_NULLPTR;
}

void QUserWidget::updateActiveUser(const QROSInterface_HSActive &hsau, QHSAD_WidgetItem *item)
{
	item->setActiveData(hsau);
}

void QUserWidget::updateActiveUser(const QROSInterface_HSActive &hsau)
{

}

void QUserWidget::addActiveUser(const QROSInterface_HSActive &hsau)
{
	QHSUD_WidgetItem *item = findUserDataItem(QHSAD_WidgetItem::IP, hsau.userName());
	Q_ASSERT(item);
	if( item )
	{
		QHSAD_WidgetItem *au_item = new QHSAD_WidgetItem();
		item->addChild(au_item);
		updateActiveUser(hsau, au_item);
	}
}

void QUserWidget::onUserDataReceived(const QROSInterface_HSUsers &hsud, bool firstDeploy)
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

void QUserWidget::onUserDataDeleted(const QROSInterface_HSUsers &hsud)
{

}

void QUserWidget::onActiveUserReceived(const QROSInterface_HSActive &hsau, bool firstDeploy)
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

void QUserWidget::onActiveUserDeleted(const QROSInterface_HSActive &hsau)
{
	// In this case, ROS only give us the ID.
	QHSAD_WidgetItem *item = findActiveUserItem(QHS_WidgetItemBase::ID, hsau.id());
	if( item )
		delete item;
}

void QUserWidget::onTochDataReceived(const QROSInterface_Torch &ti)
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
			if( ip == (uint) ti.targetIP() )
			{
				auItem->setUpload(ti.rx());
				auItem->setUploaded(auItem->uploaded()+ti.rx());
				auItem->setDownload(ti.tx());
				auItem->setDownloaded(auItem->downloaded()+ti.rx());
				auItem->setLastUpdate(QDateTime::currentDateTime());
			}
		}
	}
}
