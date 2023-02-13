#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QPair>
#include <QDir>
#include <QtXml/QDomDocument>
#include "drawlines.h"


#undef GET_GREY_IMG //необходимо для создания картинки в градациях серого

#define mIMG_WIDTH   720 // ширина
#define mIMG_HEIGHT   270 // высота


class ProcessImage : public QObject
{
    Q_OBJECT
private:
    QByteArray rawImg;

    QVector<double> averInten;
    double wholeAverInten;

    int counter = 0;
    int* traverseNode(const QDomNode& );
    QDomDocument createXmlFile(QList<QPair<int/*x0*/, int/*x1*/>>&, QPair<int/*y0*/,int/*y1*/>&);
    QDomElement createDiode(QDomDocument&, QPair<int/*x0*/, int/*x1*/>&, QPair<int/*y0*/,int/*y1*/>&);
    QDomElement makeElement(QDomDocument&, const QString&, const QString&, const QString&);

public:
    int threshold;
    QImage *prcsimg;
    QImage *srcimg;
    ProcessImage();
    ~ProcessImage();

    void loadRawImg(const QString& filename);
    void loadPixImg();
//only for simple look for coord x0,y0, x1,y1 of laser diodes!!!
    void removeColors();
//only for test!!!!
    void saveRawImg(const QString& filename);
    void paintImg(const QString& filename);


    QList<QPair <int, int>> getBoundaryRecX(QVector<int> recX);
    void setCoorLaserDiodes(int yBottom, int yTop, QVector<int> recX);

    QVector<double> getAverInten(const QString&);
    double getWholeAverInten(const QString&);

    QList<QPair<int, int>> readXml();

public slots :
    void slot_FileForAver(const QString&);

signals :
    void sig_SendAverInten(const QVector<double>&, int start_pos = -1);
    void sig_SendAverWhole(double point, int start_pos = -1);
};



#endif // PROCESSIMAGE_H
