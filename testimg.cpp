#include "testimg.h"
#include "processimage.h"
#include <QString>

TestImg::TestImg() : ProcessImage()
{

    struct Img *mask1 = new Img(new int [9]{0,0,0,0,255,255,0,0,0},3,3,1,1);
    QString filename1 = "D:\\Qt\\Images\\test1_dil.bmp";    //source
    QString filenameRes1 = "D:\\Qt\\Images\\test1_res.bmp"; //true after dilatation





}

void TestImg::runTest(void (*f)(), const QString &funcName)
{
    qDebug() << "start" << funcName;
    f();

}

bool TestImg::check(QString filename, QString filenameRes)
{
    QImage *testImg = new QImage();
    QImage *trueImg = new QImage();
    testImg->load(filename);
    trueImg->load(filenameRes);
    for (int i = 0; i < testImg->width(); i++){
        for (int j = 0; j < testImg->height(); j++){
            if (testImg->pixelColor(i, j) == trueImg->pixelColor(i, j))
                return true;
            else
                return false;
        }
    }
}

void TestImg::testDilatation(struct Img *testMask, QString filename, QString filenameRes, int numTest)
{
    {
        setMask(testMask);
        loadImg(filename);
        dilatation (filename);
        ASSERT_EQUAL(check(filename,filenameRes),true,numTest);
    }

}

void testDilatation() {
    {   //test1
        struct Img *mask1 = new Img(new int [9]{0,0,0,0,255,255,0,0,0},3,3,1,1);
        QString filename1 = "D:\\Qt\\Images\\test1_dil.bmp";    //source
        QString filenameRes1 = "D:\\Qt\\Images\\test1_res.bmp"; //true after dilatation

    }
}


TestImg::~TestImg()
{

}
