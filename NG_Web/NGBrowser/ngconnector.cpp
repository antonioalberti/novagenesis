/*
 * Autor: Daniel Fussia
 */
#include "ngconnector.h"
#include "global.h"

/* Adaptors */
#define NGBROWSER_SERVICE_NAME   "com.NGManager.NGBrowser"
#define NGBROWSER_OBJECT_PATH    "/NGConnectorObject"

/* Interfaces */
#define COMMUNICATOR_SERVICE_NAME   "com.NGManager.Communicator"
#define COMMUNICATOR_OBJECT_PATH    "/NGManagerObject"

NGConnector*
NGConnector::instance = NULL;

NGConnector::NGConnector(QObject *parent):
    caAdaptor(NULL),
    connection(QDBusConnection::sessionBus()),
    ngci(NULL)
{
    if( !connection.isConnected() ){
        qFatal("DBus nao conectado.");
    }
    if (!connection.interface()->isServiceRegistered(NGBROWSER_SERVICE_NAME) &&
        connection.registerService(NGBROWSER_SERVICE_NAME))
    {
        qInfo(NGBROWSER_SERVICE_NAME" esta registrado.");
    } else {
        qFatal("Impossivel registrar: "NGBROWSER_SERVICE_NAME);
    }
}

NGConnector* NGConnector::getInstance()
{
    if( instance == NULL ){
        instance = new NGConnector;
    }
    return instance;
}

void NGConnector::registerAdaptor()
{
    caAdaptor = new ConnectionAdaptor(this);
    if (!connection.registerObject(NGBROWSER_OBJECT_PATH, caAdaptor,
                                   QDBusConnection::ExportAllSignals|QDBusConnection::ExportAllSlots))
    {
        qFatal("Impossivel registrar: MessageAdaptor");
    }
}

void NGConnector::registerInterfaces()
{
    bool allRegistered = false;
    while(!allRegistered){
        ngci = new NGManagerCommunicationInterface(COMMUNICATOR_SERVICE_NAME,COMMUNICATOR_OBJECT_PATH,connection);
        if(ngci->isValid()){
            allRegistered = true;
        } else {
            qInfo() << "Waiting for interface: " << NGManagerCommunicationInterface::staticInterfaceName();
            delete ngci;
        }
        QThread::sleep(1);
    }
}

#define TIMEOUT_SHOWBUG     7 //seg
void NGConnector::checkFiles(QStringList arrFiles)
{
    unsigned int i=0;
    QString path = NGFILES;
    int j=TIMEOUT_SHOWBUG;
    while(i<arrFiles.size()){
        QString file = path+"/"+arrFiles.at(i);
        if( QFileInfo(file).exists() ) {
            i++;
            j = TIMEOUT_SHOWBUG;
        } else {
            emit showMessage("Waiting for: "+arrFiles.at(i));
            qInfo() << "Waiting for: "+file;
            if( j < 0 ){
                qInfo() << "FIXME ::Excesso da demora pode ser porcausa de uma linha em branco no arquivo publicado:: FIXME";
            } else {
                j--;
            }
            QThread::sleep(1);
        }
    }
}

void NGConnector::filterResultHash(QStringList* arrFiles)
{
    QStringList arrFilestmp(*arrFiles);
    arrFiles->clear();    
    for(unsigned int i=0;i<arrFilestmp.size();i++)
    {
        /* Doesn't add Duplicated */
        if( !arrFiles->contains(arrFilestmp.at(i)) ){
            /* Doesn't BID, OSID, PID, and others '_' */
            if( arrFilestmp.at(i).indexOf("_") == -1 ){
                /* Get pure file */
                arrFiles->push_back(arrFilestmp.at(i));
            }
        }
    }
}

void NGConnector::complete(QString str)
{
    QJsonParseError error;
    QJsonObject doc = QJsonDocument::fromJson(str.toUtf8(),&error).object();
    if( error.offset == QJsonParseError::NoError )
    {
        QJsonArray arrWordlist = doc["wordlist"].toArray();
        QStringList arrStrWordlist;
        for (int i = 0; i < arrWordlist.size(); ++i) {
            arrStrWordlist.push_back(arrWordlist.at(i).toString());            
        }
        qInfo() << "Before Filter: " << arrStrWordlist;
        filterResultHash(&arrStrWordlist);
//        qInfo() << "After Filter: " << arrStrWordlist;
        checkFiles(arrStrWordlist);        
        emit completeProcess(arrStrWordlist);
        emit showMessage("");
    }
}

void NGConnector::searchByLiteral(QString phrase)
{
    QStringList wordlist = phrase.split(" ");
    QJsonArray arrWordlist;
    for (int i = 0; i < wordlist.size(); ++i) {
        if( !((QString)wordlist.at(i)).isEmpty() ){
            arrWordlist.push_back(wordlist.at(i));
        }
    }
    if( arrWordlist.size() > 0 ){
        QJsonObject root;
        root.insert("wordlist",arrWordlist);
        QString strJson = QJsonDocument(root).toJson();
        ngci->SearchByLiteral(strJson);
    }
}

void NGConnector::searchByMurmur(QString murmur)
{
    if( !murmur.isEmpty() ){
        QJsonObject root;
        root.insert("word",murmur);
        QString strJson = QJsonDocument(root).toJson();
        ngci->SearchByMurmur(strJson);
    }
}














