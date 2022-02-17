/*
 * Autor: Daniel Fussia
 */
#ifndef NGCONNECTOR_H
#define NGCONNECTOR_H

#include <QtDBus>
#include "NGConnectorIf.h"
#include "ngconnectorif_adaptor.h"
#include "ngcommunication_interface.h"

class NGConnector: public NGConnectorIf
{
    Q_OBJECT
public:
    static NGConnector* getInstance();
    void registerAdaptor();
    void registerInterfaces();    

private:
    QDBusConnection connection;
    ConnectionAdaptor* caAdaptor;
    static NGConnector* instance;    

    NGManagerCommunicationInterface* ngci;

private:
    explicit NGConnector(QObject* parent=0);
    void checkFiles(QStringList arrFiles);
    void filterResultHash(QStringList* arrFiles);

    // NGConnectorIf interface
public slots:
    void complete(QString str);
    void searchByLiteral(QString phrase);
    void searchByMurmur(QString murmur);

signals:
    void showMessage(QString str);
    void completeProcess(QStringList arrFiles);
};

#endif // NGCONNECTOR_H
