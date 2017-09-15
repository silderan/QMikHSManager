#ifndef DLGROUTERCONFIG_H
#define DLGROUTERCONFIG_H

#include <QDialog>
#include "qconfigdata.h"

namespace Ui
{
	class DlgRouterConfig;
}

class DlgRouterConfig : public QDialog
{
	Q_OBJECT
	Ui::DlgRouterConfig *ui;
	QConfigRouterData &routerData;

	void ShowError(const QString &msg);
public:
	explicit DlgRouterConfig(QConfigRouterData &router_data, QWidget *parent = 0);
	~DlgRouterConfig();

private slots:
	void on_btOk_clicked();
};

#endif // DLGROUTERCONFIG_H
