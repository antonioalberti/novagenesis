/*
 * Autor: Daniel Fussia
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QToolButton>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebFrame>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeLocation();
    bool doOpenPage(QString sUrl);
    bool doFetch(QString sUrl);
    bool doProcess(QString sUrl);
    bool doProcessNgs(QString content);
    bool doProcessNgu(QString content);
    void loadContent(QUrl qurl);
    void showMessage(QString message);
    void completeProcess(QStringList arrFiles);
    QString completeProcessNgs(QStringList arrFiles);
    QString completeProcessNgu(QStringList arrFiles);


    void 	iconChanged();
    void 	linkClicked(const QUrl & url);
    void 	loadFinished(bool ok);
    void 	loadProgress(int progress);
    void 	loadStarted();
    void 	selectionChanged();
    void 	statusBarMessage(const QString & text);
    void 	titleChanged(const QString & title);
    void 	urlChanged(const QUrl & url);
    void    download(const QNetworkRequest &request);
    void    unsupportedContent(QNetworkReply * reply);
    void    finished(QNetworkReply * reply);

private:    
    Ui::MainWindow *ui;
    void addToolBar();
    QLineEdit *locationEdit;
    QToolButton *configButton;    
    QString type;
    bool isFetch;
    int waiting;
    QUrl lastLoadContent;
};

#endif // MAINWINDOW_H
