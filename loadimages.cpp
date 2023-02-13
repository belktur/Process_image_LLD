#include "loadimages.h"

LoadImages::LoadImages()
{   
    files = new QFileInfoList();
}


void LoadImages::getFiles(const QString& dirName1)
{
    qDebug() << dirName1;
    QDir curDir(dirName);
    if (dirName1.size() > 0) {
        curDir.setPath(dirName1);
    }
    QStringList filters;
    filters << "*.bin8";
    curDir.setNameFilters(filters);
    curDir.setFilter(QDir::Files | QDir::NoSymLinks);
    *files = curDir.entryInfoList();

    getAverageIntenDiod(*files);
}


void LoadImages::getAverageIntenDiod(QFileInfoList &files)
{
    writeAverToFile({}, 0);
    clearFiles();

    for (auto &file : files)
    {
        QString fullFileName = file.absoluteFilePath();
        //check extenstion of file
        assert(strncmp(&fullFileName.toUtf8().data()[fullFileName.size()-4], "bin8", 4) == 0);

//        class ProcessImage* OneImg = new class ProcessImage();

        QFile resFileInten(FILE_AVER_NAME);
        resFileInten.open(QIODevice::Append);
        resFileInten.write(file.fileName().toUtf8());
        resFileInten.write("\t");
        resFileInten.close();
        emit sig_FileForAVer(fullFileName);
        qDebug() << "sig = " << fullFileName;
//        writeAverToFile(OneImg->getAverInten(fullFileName));
//        writeWholeAverToFile(OneImg->getWholeAverInten(fullFileName));
//        delete OneImg;
    }
    filtration(FILE_AVER_NAME, wWin);
}

void LoadImages::clearFiles()
{
    // deleting previous results from a file
    QFile file(FILE_AVER_NAME);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.close();
    // deleting previous results from a file
    QFile file_2("WholeAverageInt.txt");
    file_2.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file_2.close();

}

void LoadImages::writeAverToFile(const QVector<double>& averInt, int start_pos)
{
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
            resFileInten.write(QString("\t%1").arg(QString::number(averInt[i])).toUtf8());
        }
        resFileInten.putChar('\n');
        resFileInten.close();
    }

}

void LoadImages::filtration(const QString &filename, int wwin)
{
    QFile fileFilter(filename);
    fileFilter.open(QIODevice::ReadOnly);
    QByteArray fileContent = fileFilter.readAll();
    fileFilter.close();

    QByteArrayList stringsFile = fileContent.split('\n');

    QFile fileFilterRes("FilterResults.txt");
    fileFilterRes.open(QIODevice::WriteOnly);

    int size = stringsFile.size();
    double aver[64];

    int num = 1;
    int averSize = 0;

    for (int i = 0; i < size - wwin; i++){
        for (int j = i; j < wwin + i; j++){
            QByteArrayList colValue = stringsFile[j].split('\t');
            averSize = colValue.size();
            if (colValue.size() < 2){
                averSize = -1;
                continue;
            }
            for (int k = 0; k < colValue.size() - 2; k++){
                aver[k] += colValue[k+2].toDouble() / (double)wwin;
            }
        }
        fileFilterRes.write(QString("%1").arg(num).toUtf8());
        for (int j = 0; j < averSize; j++){
            fileFilterRes.write(QString("\t%1").arg(aver[j]).toUtf8());
        }
        num++;
        fileFilterRes.write("\n");
        memset(aver, 0, averSize * sizeof(double));
    }
    fileFilterRes.close();
}


void LoadImages::writeWholeAverToFile(const double wholeAverInt, int start_pos)
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

LoadImages::~LoadImages()
{
    delete files;
}

void LoadImages::slot_WriteAverInten(const QVector<double> &averInt, int start_pos)
{
    writeAverToFile(averInt, start_pos);
}

void LoadImages::slot_WriteWholeAverInten(const double point, int start_pos)
{
    writeWholeAverToFile(point, start_pos);
}
