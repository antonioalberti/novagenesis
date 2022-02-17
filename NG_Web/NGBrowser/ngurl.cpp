/*
 * Autor: Daniel Fussia
 */
#include "ngurl.h"
#include <QStringList>

NGUrl::NGUrl()
{
}

bool NGUrl::isNGUrl(QString url)
{
    QString urlType = this->getUrlType(url);
    if( urlType == "ngs" || urlType == "ngu" || urlType == "ng-sha1" || urlType == "ng-md5"){
        return true;
    }
    return false;
}

QString NGUrl::requestNGContent(QString url)
{
    QString urlType = this->getUrlType(url);
    QString urlContent = this->getUrlContent(url);
    return this->path+urlType+"/"+urlContent+".html";
}

QString NGUrl::getUrlContent(QString url)
{
    QStringList arrUrl = url.split("://");
    if( arrUrl.size() > 1 ){
        return arrUrl.at(1).toLocal8Bit().constData();
    }
    return "";
}

QString NGUrl::getUrlType(QString url)
{
    QStringList arrUrl = url.split("://");
    if( arrUrl.size() > 0 ){
        return arrUrl.at(0).toLocal8Bit().constData();
    }
    return "";
}
