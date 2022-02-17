/*
 * Autor: Daniel Fussia
 */
#include "ngtemplate.h"
#include "global.h"
#include <QFile>
#include <QTextStream>

#define NGRESULTFILE "ngresult.html"

NGTemplate::NGTemplate():
    outputItem("")
{
    QFile f1(NGBROWSERTMPL"/ngs-mainwindow-tmpl.html");
    if ( f1.open(QFile::ReadOnly | QFile::Text) ){
        QTextStream in(&f1);
        mainwindowtmpl = in.readAll();
        f1.close();
    }

    QFile f2(NGBROWSERTMPL"/ngs-item-mainwindow-tmpl.html");
    if ( f2.open(QFile::ReadOnly | QFile::Text) ){
        QTextStream in(&f2);
        itemtmpl = in.readAll();
        f2.close();
    }
}

void NGTemplate::addItem(QString title,QString link,QString description)
{
    QString renderItem = itemtmpl;
    renderItem.replace("#title#",title);
    renderItem.replace("#link#",link);
    renderItem.replace("#description#",description);
    outputItem += renderItem;
    //qInfo() << renderItem << " " << title;
}

QString NGTemplate::dump()
{
    QString output = mainwindowtmpl.replace("#ngs-item-mainwindow-tmpl#",outputItem);
    QFile f(NGFILES"/"NGRESULTFILE);
    if ( f.open(QFile::WriteOnly | QFile::Truncate) ){
        QTextStream os(&f);
        os << output;
        f.close();
    }

    return NGRESULTFILE;
}
