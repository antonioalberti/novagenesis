/*
 * Autor: Daniel Fussia
 */
#ifndef NGCONNECTORIF_H
#define NGCONNECTORIF_H

#include <QtCore/QObject>

class NGConnectorIf: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "NGConnector.Connection")

public slots:
    virtual void complete(QString str)=0;


};

#endif // NGCONNECTORIF_H
