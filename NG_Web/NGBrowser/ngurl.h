/*
 * Autor: Daniel Fussia
 */
#ifndef NGURL_H
#define NGURL_H

#include <QString>
#include "global.h"

class NGUrl
{
public:
    NGUrl();
    bool isNGUrl(QString url);
    QString requestNGContent(QString url);
    QString getUrlType(QString url);
    QString getUrlContent(QString url);

private:
    QString path = "file://"NGFILES;
};

#endif // NGURL_H
