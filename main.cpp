#include "mainwindow.h"
#include <QApplication>

#ifndef QT_NO_DEBUG
#include "QROSInterface_Base.h"
#include "Utils.h"
#endif

int main(int argc, char *argv[])
{
#ifndef QT_NO_DEBUG
	Utils::UnitTests();

	Q_ASSERT( QROSVersion("5.29").toString() == "5.29.0" );
	Q_ASSERT( QROSVersion("1.2.3").toString() == "1.2.3" );
	Q_ASSERT( QROSVersion("5.0").toString() == "5.0.0" );
	Q_ASSERT( QROSVersion("qewr1msiai33akjfa21lñkl0").toString() == "1.33.21" );
	Q_ASSERT( QROSVersion("").toString() == "0.0.0" );

	Q_ASSERT( QROSVersion("1.2.3") == QROSVersion("1.2.3") );
	Q_ASSERT( QROSVersion("5.0") < QROSVersion("5.29") );
	Q_ASSERT( QROSVersion("1.2.3") < QROSVersion("5.29") );
	Q_ASSERT( QROSVersion("5.1") > QROSVersion("5.0") );
	Q_ASSERT( QROSVersion("5.0.1") > QROSVersion("5.0") );
	Q_ASSERT( QROSVersion() == QROSVersion() );

	Q_ASSERT( QROSVersion().isValid() == false );
#endif

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
