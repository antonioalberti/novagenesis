/*
 * Autor: Daniel Fussia
 */
#include "mainwindow.h"
#include <QApplication>
#include "ngconnector.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NGConnector* ngc = NGConnector::getInstance();
    ngc->registerAdaptor();
    ngc->registerInterfaces();

    MainWindow w;
    w.show();

    return a.exec();
}
