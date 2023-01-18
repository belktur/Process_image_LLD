#ifndef TESTIMG_H
#define TESTIMG_H
#include <QImage>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QPair>
#include "processimage.h"


#define RUN_TEST(func)  TestImg::runTest(func, #func)

void assert_equal(bool expr, bool expr_true, int numTest)
{
    if (numTest > 0)
        qDebug() << "Test" << numTest << " ";
    else
        qDebug() << "Wrong test number";

    if (expr == expr_true)
        qDebug() << "Passed OK";
    else
    {
        qDebug() << "Passed NO";
        qDebug() << "( " << expr << "not equal" << expr_true << " )";
    }
}
#define ASSERT_EQUAL(expr, expr_true, numTest) assert_equal((expr), (expr_true), numTest)


class TestImg : public ProcessImage
{
private:
    QImage *testimg;
    struct Img* TMask;
public:
    TestImg();
    static void runTest(void (*f)(void), const QString & funcName);
    bool check(QString filename, QString filename_res);
    void testDilatation(struct Img *testMask, QString filename, QString filenameRes, int numTest);
    void testErosian(struct Img *testMask, QString filename, QString filenameRes);




    ~TestImg();
};

void testDilatation();

#endif // TESTIMG_H
