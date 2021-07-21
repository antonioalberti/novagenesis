/*
 * Autor: Daniel Fussia
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ngurl.h"
#include "ngconnector.h"
#include "ngtemplate.h"

#include <QMimeDatabase>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    type(""),
    isFetch(false),
    waiting(0)
{
    ui->setupUi(this);
    this->addToolBar();
    setCentralWidget(ui->webView);
    connect(NGConnector::getInstance(),SIGNAL(showMessage(QString)),this,SLOT(showMessage(QString)));
    connect(NGConnector::getInstance(),SIGNAL(completeProcess(QStringList)),this,SLOT(completeProcess(QStringList)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToolBar(){
    QWebView *view = ui->webView;
    // Action buttons
    ui->toolBar->addAction(view->pageAction(QWebPage::Back));
    ui->toolBar->addAction(view->pageAction(QWebPage::Forward));
    // URL field
    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    locationEdit->setAutoFillBackground(true);    
    ui->toolBar->addWidget(locationEdit);
    // URL field events
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
    // More Action buttons
    ui->toolBar->addAction(view->pageAction(QWebPage::Stop));
    // Configuration Button
    configButton = new QToolButton(this);
    QFont fontConfig;
    fontConfig.setBold(true);
    fontConfig.setPixelSize(18);
    configButton->setText("=");
    configButton->setFont(fontConfig);
    configButton->setMaximumWidth(26);
    ui->toolBar->addWidget(configButton);

    view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

//    connect(view,SIGNAL(iconChanged()),this,SLOT(iconChanged()));
    connect(view,SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
    connect(view,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
//    connect(view,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));
//    connect(view,SIGNAL(loadStarted()),this,SLOT(loadStarted()));
//    connect(view,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
//    connect(view,SIGNAL(statusBarMessage(QString)),this,SLOT(statusBarMessage(QString)));
//    connect(view,SIGNAL(titleChanged(QString)),this,SLOT(titleChanged(QString)));
//    connect(view,SIGNAL(urlChanged(QUrl)),this,SLOT(urlChanged(QUrl)));

    view->page()->setForwardUnsupportedContent(true);
    connect(view->page(),SIGNAL(downloadRequested(QNetworkRequest)),this,SLOT(download(QNetworkRequest)));
    connect(view->page(),SIGNAL(unsupportedContent(QNetworkReply*)),this,SLOT(unsupportedContent(QNetworkReply*)));
    connect(view->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));
}

/*
 * file:///home/danf/htdocs/index.html
 * ng-sha1://fedc43a45d3a5c0d4ee84f779d7e5b9f43146dfa
 * ng-md5://acdde28eafd13432c7522dc157c31d8f
 * file:///home/william/newWorkspace/novagenesis/IO/NGAppCommunicator/D861C0EFE444E9113992962F81BAFC72
 */

void MainWindow::changeLocation()
{    

    QString sUrl = locationEdit->text();

    doOpenPage(sUrl);
/*
    qInfo("Abrindo.. %s",sUrl.toLocal8Bit().constData());
    ui->webView->load(sUrl);
//    ui->webView->loadProgress();
    ui->webView->show();
    ui->webView->setFocus();
*/
}

bool MainWindow::doOpenPage(QString sUrl)
{
    qInfo() << "OpeningPage: " << sUrl;
    isFetch = false;
    return doProcess(sUrl);
}

bool MainWindow::doFetch(QString sUrl)
{
    qInfo() << "Fetching: " << sUrl;
    isFetch = true;
    return doProcess(sUrl);
}

bool MainWindow::doProcess(QString sUrl)
{
    ui->statusBar->showMessage("Loading...");

    NGUrl ngu;

    bool hasError = false;

    if( ngu.isNGUrl(sUrl) ){
        QString content = ngu.getUrlContent(sUrl);
        type = ngu.getUrlType(sUrl);

        if( type == "ngs" ){
            hasError = doProcessNgs(content);
        } else if( type == "ngu" ){
            hasError = doProcessNgu(content);
        } else {
            ui->statusBar->showMessage("Type '"+type+"' not implemented.");
            hasError = true;
        }

//        if( !displayMessage ){
//            ui->statusBar->showMessage("Requisition error. Maybe NGCommunicator isn't running.");
//        }

#if 0
// Enable for test to open file direct
    } else if( sUrl.startsWith("file://") ) {
        QUrl qurl = QUrl(sUrl);
        QFile f(qurl.toLocalFile());
        if( f.open(QFile::ReadOnly) ){
            QByteArray qba = f.readAll();
            f.close();
            QMimeDatabase qmdb;
            QMimeType qmt = qmdb.mimeTypeForUrl(qurl);
            QString path = qurl.url();
            path.replace(qurl.fileName(),"");
            ui->webView->setContent(qba,qmt.name(),path);
            ui->webView->show();
            ui->webView->setFocus();
            qInfo() << "Opening: " << path << " / " << qurl.fileName() << " " << qmt.name();
        } else {
            qInfo() << "Error" << qurl.toLocalFile();
        }
#endif
    } else {
        ui->statusBar->showMessage("Syntax error. Try: ngs://test my ngbrowser");
        hasError = true;
    }

    return hasError;
}

bool MainWindow::doProcessNgs(QString content)
{
    if(content == "test my ngbrowser"){
        ui->statusBar->showMessage("Greetings!! Test Ok!!!");
        return false;
    }
    NGConnector* ngc = NGConnector::getInstance();
    ngc->searchByLiteral(content);
    return false;
}

bool MainWindow::doProcessNgu(QString content)
{
    NGConnector* ngc = NGConnector::getInstance();
    ngc->searchByMurmur(content);
    return false;
}

void MainWindow::loadContent(QUrl qurl)
{
    if( qurl.isLocalFile() ){
        QFile f(qurl.toLocalFile());
        if( f.open(QFile::ReadOnly) ){
            QByteArray qba = f.readAll();
            f.close();
            QMimeDatabase qmdb;
            QMimeType qmt = qmdb.mimeTypeForUrl(qurl);
            QString path = qurl.url();
            path.replace(qurl.fileName(),"");
            ui->webView->setContent(qba,qmt.name(),path);
            ui->webView->show();
            ui->webView->setFocus();
            qInfo() << "Opening: " << path << "/" << qurl.fileName() << " mime-type: " << qmt.name();
            lastLoadContent = qurl;
        } else {
            qInfo() << "Error " << qurl.toLocalFile();
        }
    }
}

void MainWindow::showMessage(QString message)
{
    ui->statusBar->showMessage(message);
}

void MainWindow::completeProcess(QStringList arrFiles)
{
    QString sUrl;
    if( type == "ngs" ){
        sUrl = completeProcessNgs(arrFiles);
    } else if( type == "ngu" ){
        sUrl = completeProcessNgu(arrFiles);
    }
    if( !isFetch && !sUrl.isEmpty() ){
        QUrl qurl = QUrl("file://"+sUrl);
        loadContent(qurl);
    } else if( isFetch ){
        if( --waiting == 0 ){
            qInfo() << "Reloading";
            loadContent(lastLoadContent);
            isFetch = false;
        }
    }
}

QString MainWindow::completeProcessNgs(QStringList arrFiles)
{
    showMessage("Rendering Results..");
    QString path = NGFILES;
    NGTemplate ngt;
    for (unsigned int i = 0; i < arrFiles.size(); ++i) {
        QString file = path+"/"+arrFiles.at(i);
        QFile f(file);
        if ( f.open(QFile::ReadOnly | QFile::Text) ){
            QTextStream in(&f);
            QString buffFile = QString(in.readAll());
            f.close();
            QJsonParseError error;
            QJsonObject doc = QJsonDocument::fromJson(buffFile.toUtf8(),&error).object();
            if( error.offset == QJsonParseError::NoError ){
                //qInfo() << doc["title"].toString() << doc["link"].toString() << doc["description"].toString();
                ngt.addItem(
                      doc["title"].toString(),
                      doc["link"].toString(),
                      doc["description"].toString()
                );
            }
        }
    }    
    QString sUrl = path+"/"+ngt.dump();
    showMessage("Finished.");
    return sUrl;
}

QString MainWindow::completeProcessNgu(QStringList arrFiles)
{
    showMessage("Rendering Results..");
    QString path = NGFILES;
    QString sUrl = "";
    if(arrFiles.size()>0){
        sUrl = path+"/"+arrFiles.at(0);
    } else {
        //sUrl = "page_error.html"; // TODO
    }
    showMessage("Finished.");
    return sUrl;
}

void MainWindow::iconChanged()
{
    qInfo("iconChanged");
}

void MainWindow::linkClicked(const QUrl &url)
{    
    doProcess("ngu://"+url.fileName());
}

void MainWindow::loadFinished(bool ok)
{
    qInfo() << "loadFinished: " << ok;
}

void MainWindow::loadProgress(int progress)
{
    qInfo("loadProgress");
}

void MainWindow::loadStarted()
{
    qInfo("loadStarted");
}

void MainWindow::selectionChanged()
{
    qInfo("selectionChanged");
}

void MainWindow::statusBarMessage(const QString &text)
{
    qInfo("statusBarMessage");
}

void MainWindow::titleChanged(const QString &title)
{
    qInfo("titleChanged");
}

void MainWindow::urlChanged(const QUrl &url)
{
    qInfo("urlChanged");
}

void MainWindow::download(const QNetworkRequest &request){
    qInfo()<<"Download Requested: "<<request.url();
}

void MainWindow::unsupportedContent(QNetworkReply * reply){

    qInfo()<<"Unsupported Content: "<<reply->url();

}

void MainWindow::finished(QNetworkReply *reply)
{
    if( reply->url().isLocalFile() ){
        QString filename = reply->url().fileName();
        qInfo() << "Path: " << reply->url().path();
        doFetch("ngu://"+filename);
        waiting++;
    }
}

