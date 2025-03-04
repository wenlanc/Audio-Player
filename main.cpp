#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings::setDefaultFormat(QSettings::IniFormat); // personal preference
    qApp->setOrganizationName("EventsHD Soft");
    qApp->setApplicationName("MU Player");
    MainWindow w;
    w.showMaximized();
    w.show();

    return a.exec();
}
