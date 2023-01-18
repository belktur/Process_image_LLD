#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H
#include <QImage>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QPair>
#include <QDir>
#include <QtXml/QDomDocument>

#undef GET_GREY_IMG //необходимо для создания картинки в градациях серого


#define THRESHOLD_GRAY  0x1A
#define mIMG_WIDTH   720
#define mIMG_HEIGHT   270
#define CORR_LEN_X    3
#define CORR_LEN_Y    1
#define SPACE_BTW_D   8


//необходимо для расширения области поиска средней интенсивности лазерного диода!!!
#define REC_Y_UPPER_SHIFT   -35   //must negative
#define REC_Y_DOWN_SHIFT    35
#define REC_X_LEFT_SHIFT    -4   //must negative
#define REC_X_RIGHT_SHIFT   4



struct Rec {
    int x0;
    int x1;
    int y0;
    int y1;
};

QDomDocument createXmlFile(QList<QPair<int/*x0*/, int/*x1*/>>&, QPair<int/*y0*/,int/*y1*/>&);
QDomElement createDiode(QDomDocument&, QPair<int/*x0*/, int/*x1*/>&, QPair<int/*y0*/,int/*y1*/>&);
QDomElement makeElement(QDomDocument&, const QString&, const QString&, const QString&);



class ProcessImage
{
private:
    QByteArray rawImg;
    QList<Rec> listCoorDiodes;
    QList<QPair<int, int>> finalCoord;

    QImage *srcimg;
    QImage *prcsimg;
    QVector<double> averInten;
    double wholeAverInten;
    int counter = 0;


    QList<QPair<int,int>> getAllBoundaryY();    //return *??? &???
    QPair<int/*y0*/,int/*y1*/> getBoundaryRecY(const QList<QPair<int, int>>& boundaries);

    QList<QPair<int,int>> getAllBoundaryX();    //return *??? &???
    QList<QPair<int/*x0*/, int/*x1*/>> getBoundaryRecX(QList<QPair<int,int>>&);
    QList<QPair<int/*x0*/, int/*x1*/>> corBoundaryRecX(QList<QPair<int,int>>&);
    QList<QPair<int, int>> extendCoordX(QList<QPair<int,int>>&);

public:
    ProcessImage();
    ~ProcessImage();

    void loadRawImg(const QString& filename);
//only for simple look for coord x0,y0, x1,y1 of laser diodes!!!
    void removeColors();
//only for test!!!!
    void saveRawImg(const QString& filename);
    void binRawImg();
    void getCoorLaserDiodes();
    QVector<double> getAverInten(const QString&);
    double getWholeAverInten(const QString&);
    void threshold();
    void drawHistogram();
    void readXml();
    int* traverseNode(const QDomNode& );


};



#endif // PROCESSIMAGE_H
