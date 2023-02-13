#ifndef LOADIMAGES_H
#define LOADIMAGES_H

#include <QObject>
#include <QString>
#include <iostream>
#include <QtXml/QDomDocument>
#include "processimage.h"

const QString FILE_AVER_NAME = QString("AverageInt.txt");

class LoadImages : public QObject
{
    Q_OBJECT
private:
    QString dirName =  "D:\\Qt\\Projects\\f25_I30_t300_T30";
    //width of the window for filter
    const int wWin = 15;
    QFileInfoList* files;

public:
    LoadImages();
    void getFiles(const QString& dirName1 = "");
    void getAverageIntenDiod(QFileInfoList &files);
    void clearFiles();
    void writeAverToFile(const QVector<double>& averInt, int start_pos = -1);
    void writeWholeAverToFile(const double wholeAverInt, int start_pos = -1);
    void filtration(const QString &filename, int wwin);
    ~LoadImages();

signals :
    void sig_FileForAVer(const QString&);

public slots :
    void slot_WriteAverInten(const QVector<double>& averInt, int start_pos = -1);
    void slot_WriteWholeAverInten(const double, int start_pos = -1);

};

#endif // LOADIMAGES_H
