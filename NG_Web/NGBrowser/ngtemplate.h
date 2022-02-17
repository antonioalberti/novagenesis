/*
 * Autor: Daniel Fussia
 */
#ifndef NGTEMPLATE_H
#define NGTEMPLATE_H

#include <QObject>

class NGTemplate
{
public:
    NGTemplate();
    void addItem(QString title,QString link,QString description);
    QString dump();
private:
    QString mainwindowtmpl;
    QString itemtmpl;
    QString outputItem;
};

#endif // NGTEMPLATE_H
