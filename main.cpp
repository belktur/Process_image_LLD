#include "mainwindow.h"
#include <QApplication>
#include "processimage.h"
#include "testimg.h"
#include <iostream>
#include <QString>
#include <QtXml/QDomDocument>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qDebug() << "our program will start from " << QDir::currentPath();

    w.show();


    return a.exec();
}
