#include "DlgRouterConfig.h"
#include "ui_DlgRouterConfig.h"

#include <QMessageBox>

void DlgRouterConfig::ShowError(const QString &msg)
{
	QMessageBox::information(this, tr("Configurando router"), msg, QMessageBox::Ok );
}

DlgRouterConfig::DlgRouterConfig(QConfigRouterData &router_data, QWidget *parent) : QDialog(parent),
	ui(new Ui::DlgRouterConfig), routerData(router_data)
{
	ui->setupUi(this);
	setWindowTitle(QString("#%1 %2").arg(routerData.id, routerData.name));
	ui->leName->setText(routerData.name.isEmpty() ? QString("Router_%1").arg(routerData.id) : routerData.name);
	ui->leIP->setText(routerData.IP);
	ui->lePass->setText(routerData.upass);
	ui->leUser->setText(routerData.uname);
	ui->sbPort->setValue(routerData.port ? routerData.port : 8291);
}

DlgRouterConfig::~DlgRouterConfig()
{
	delete ui;
}

void DlgRouterConfig::on_btOk_clicked()
{
	if( ui->leIP->text().isEmpty() )
		ShowError(tr("Falta la dirección IP o FQDN del router.") );
	else
	if( ui->leIP->text().contains(" ") )
		ShowError(tr("La dirección IP o FQDN contiene un espacio en blanco y no está permitido.") );
	else
	if( ui->leName->text().isEmpty() )
		ShowError(tr("Falta un nombre arbitrario que identifique al router.") );
	else
	if( ui->leUser->text().isEmpty() )
		ShowError(tr("Falta el nombre de usuario para acceder al router.") );
	else
	if( ui->lePass->text().isEmpty() )
		ShowError(tr("Falta la contraseña para acceder al router.") );
	else
	{
		routerData.IP = ui->leIP->text();
		routerData.name = ui->leName->text();
		routerData.port = ui->sbPort->value();
		routerData.uname = ui->leUser->text();
		routerData.upass = ui->lePass->text();
		accept();
	}
}
