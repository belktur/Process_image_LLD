#include "mainwindow.h"

#include <QApplication>
#include "processimage.h"
#include "testimg.h"
#include <iostream>
#include <QString>
#include <QtXml/QDomDocument>


const QString FILE_AVER_NAME = QString("AverageInt.txt");
//width of the window for filter
const int WWIN = 15;


void filtration(const QString &filename, int wwin){
    QFile fileFilter(filename);
    fileFilter.open(QIODevice::ReadOnly);
    QByteArray fileContent = fileFilter.readAll();
    fileFilter.close();
    QByteArrayList stringsFile = fileContent.split('\n');

    QFile fileFilterRes("FilterResults.txt");
    fileFilterRes.open(QIODevice::WriteOnly);

    int size = stringsFile.size();
    double aver[17];
    memset(aver, 0, 17 * sizeof(double));
    int num = 1;
    for (int i = 0; i < size - wwin; i++){
        for (int j = i; j < wwin + i; j++){
            QByteArrayList colValue = stringsFile[j].split('\t');
            if (colValue.size() < 2){
                continue;
            }
            for (int k = 0; k < colValue.size(); k++){
                aver[k] += colValue[k].toInt() / (double)wwin;
            }

        }
        fileFilterRes.write(QString("%1").arg(num).toUtf8());
        for (int j = 1; j < 17; j++){

            fileFilterRes.write(QString("\t%1").arg(aver[j]).toUtf8());

        }
        num++;
        fileFilterRes.write(QString("\n").toUtf8());
        memset(aver, 0, 17 * sizeof (double));
    }
    fileFilterRes.close();
}

QFileInfoList getFiles(const QString &dirName)
{
    QDir curDir(dirName);
    QStringList filters;
    filters << "*.bin8";
    curDir.setNameFilters(filters);
    curDir.setFilter(QDir::Files | QDir::NoSymLinks);

    return curDir.entryInfoList();
}

void writeAverToFile(const QVector<double>& averInt, int start_pos = -1)
{
    static int numberLine = 0;
    if (start_pos >= 0)
        numberLine = start_pos;

    if (averInt.isEmpty())
        return;

    QFile resFileInten(FILE_AVER_NAME);
    resFileInten.open(QIODevice::Append);
    resFileInten.write(QString("%1").arg(numberLine++).toUtf8());

    for (int i = 0; i < averInt.size(); i++){
        resFileInten.write(QString("\t%1").arg(averInt[i]).toUtf8());
    }
    resFileInten.putChar('\n');
    resFileInten.close();
}

void writeWholeAverToFile(const double wholeAverInt, int start_pos = -1)
{
    if (wholeAverInt == -1){
        return;
    }
    static int numberLine = 0;
    if (start_pos >= 0)
        numberLine = start_pos;

    QFile resFileInten("WholeAverageInt.txt");
    resFileInten.open(QIODevice::Append);
    resFileInten.write(QString("%1").arg(numberLine++).toUtf8());
    resFileInten.write(QString("\t%1").arg(wholeAverInt).toUtf8());
    resFileInten.putChar('\n');
    resFileInten.close();
}

QVector<double> getAverageIntenDiod(const QString &dirName)
{
    auto files = getFiles(dirName);

    writeAverToFile({}, 0);

   // deleting previous results from a file
    QFile file(FILE_AVER_NAME);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.close();
    // deleting previous results from a file
     QFile file_2("WholeAverageInt.txt");
     file_2.open(QIODevice::WriteOnly | QIODevice::Truncate);
     file_2.close();


    for (auto &file : files)
    {
        QString fullFileName = file.absoluteFilePath();
        //check extenstion of file
        assert(strncmp(&fullFileName.toUtf8().data()[fullFileName.size()-4], "bin8", 4) == 0);


        class ProcessImage* OneImg = new class ProcessImage();

        writeAverToFile(OneImg->getAverInten(fullFileName));
        writeWholeAverToFile(OneImg->getWholeAverInten(fullFileName));
        delete OneImg;
    }

    filtration(FILE_AVER_NAME, WWIN);
    return {};
}




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qDebug() << "our program will start from " << QDir::currentPath();



    ProcessImage prImg;
    prImg.getCoorLaserDiodes();



    QString dirName =  "D:\\Qt\\Projects\\f1_I30_t300_T25";



    auto temp = getAverageIntenDiod(dirName);





    w.show();
    return a.exec();
}
