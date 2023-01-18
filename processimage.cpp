#include "processimage.h"
#include <stdio.h>
#include <QtXml>
#include <QApplication>
#include <iostream>
#include <QString>





ProcessImage::ProcessImage()
{
    srcimg = new QImage();
    prcsimg = nullptr;
}


void ProcessImage::loadRawImg(const QString &fileName)
{
    qDebug() << QDir::currentPath();

    QFile tmpFile(fileName);
    tmpFile.open(QIODevice::ReadOnly);
    rawImg = tmpFile.readAll();
    tmpFile.close();
}

void ProcessImage::removeColors()
{
    for (int i = 1; i < rawImg.size(); i++) {
        rawImg.remove(i, 2);
    }
}

void ProcessImage::saveRawImg(const QString &fileName)
{
    QFile tmpFile(fileName);
    tmpFile.open(QIODevice::WriteOnly);
    tmpFile.write(rawImg);
    tmpFile.close();

    QImage *testImg = new QImage(mIMG_WIDTH, mIMG_HEIGHT, QImage::Format_RGB888);
    int i = 0;
    for (int y = 0; y < mIMG_HEIGHT; y++) {
        for (int x = 0; x < mIMG_WIDTH; x++) {
            testImg->setPixelColor(x, y, QColor((unsigned char)rawImg.at(i),
                                                (unsigned char)rawImg.at(i),
                                                (unsigned char)rawImg.at(i)));
            i++;
        }
    }
    bool res = testImg->save(fileName + ".bmp");
    qDebug() << "bmp" << (res == true ? " saved" : " NOT saved");
}

void ProcessImage::binRawImg()
{
    for (int i = 0; i < rawImg.size(); i++) {
        if ((unsigned char)rawImg.at(i) < THRESHOLD_GRAY) {
            rawImg[i] = 0;
        }
        else {
            rawImg[i] = 0xFF;
        }
    }
}

QList<QPair<int, int>> ProcessImage::getAllBoundaryX()
{
    QList<QPair<int,int>> listBoundaries;

    QPair<int /*left*/, int /*right*/> boundary;
    int corrLen = 0;
    for (int y = 0; y < mIMG_HEIGHT; y++) {
        bool firstBoundary = true;
        for (int x = 0; x < mIMG_WIDTH-1; x++) {
            //go from black to white....
            int x0 = y*mIMG_WIDTH+x;
            int x1 = x0 + 1;
            int color_x1 = (unsigned char)rawImg.at(x1);
            int color_x0 = (unsigned char)rawImg.at(x0);

            if ((color_x1 - color_x0) > 0) {
                firstBoundary = false;
                boundary.first = x+1;
                corrLen = 0;
            }
            else if (firstBoundary == false && ((color_x1 - color_x0) < 0)) {
                //for from white to black
                firstBoundary = true;
                boundary.second = x;
                if (corrLen >= CORR_LEN_X){
                    listBoundaries.push_back(boundary);
                }
            }
            else if ((color_x1 - color_x0) < 0) {
                qDebug() << "ERROR : " << __func__ << " " << color_x1 << " : " << color_x0;
            }
            corrLen++;
        }
    }
    return listBoundaries;
}

QList<QPair<int, int>> ProcessImage::getBoundaryRecX(QList<QPair<int, int>> &boundaries)
{
    std::sort(boundaries.begin(), boundaries.end());

    for (int i = 0; i < boundaries.size() - 1; i++) {
        QPair<int,int>& curBoundary = boundaries[i];
        int j = i + 1;
        while ((j < boundaries.size()) && ((boundaries.at(j).first - curBoundary.second) < 0)) {

            if (curBoundary.first > boundaries.at(j).first) {
                curBoundary.first = boundaries.at(j).first;
            }
            if (curBoundary.second < boundaries.at(j).second) {
                curBoundary.second = boundaries.at(j).second;
            }
            j++;
        }
        boundaries.erase(boundaries.begin()+i+1, boundaries.begin()+j);
    }
   int j = 0;
   while ( j < boundaries.size() && boundaries.size() != 16){
       for (int i = 0; i < boundaries.size() - 1; i++){
           QPair<int,int> &curBoundary = boundaries[i];
           j = i + 1;
           int temp = boundaries.at(j).first - curBoundary.second;
           if ((boundaries.at(j).first - curBoundary.second) < SPACE_BTW_D){
                curBoundary.second = boundaries.at(j).second;
                boundaries.erase(boundaries.begin()+j);
            }
        }
    }

   return boundaries;
}

QList<QPair<int, int> > ProcessImage::corBoundaryRecX(QList<QPair<int, int> > &boundaries)
{
    int corLength = 0;
    int length = boundaries[0].second - boundaries[0].first;
    for (int i = 1; i < boundaries.size(); i++){
        corLength = boundaries[i].second - boundaries[i].first;
        if (corLength > length){
            length = corLength;
        }
    }
    int gapLengthLeft = 0;
    int gapLengthRight = 0;
    for (int i =1; i < boundaries.size()-1; i++){
        corLength = boundaries[i].second - boundaries[i].first;
        gapLengthLeft = boundaries[i].first - boundaries[i-1].second;
        gapLengthRight = boundaries[i+1].first - boundaries[i].second;
        while (corLength < length){
            if (gapLengthRight > gapLengthLeft){
                boundaries[i].second = boundaries[i].second + 1;
            }
            if (gapLengthRight <= gapLengthLeft){
                boundaries[i].first = boundaries[i].first - 1;
            }
            corLength = boundaries[i].second - boundaries[i].first;
            gapLengthLeft = boundaries[i].first - boundaries[i-1].second;
            gapLengthRight = boundaries[i+1].first - boundaries[i].second;
        }
    }
    while (boundaries[0].second - boundaries[0].first < length){
        boundaries[0].first = boundaries[0].first - 1;
    }
    while (boundaries[15].second - boundaries[15].first < length){
        boundaries[15].second = boundaries[15].second + 1;
    }
    return boundaries;
}


QList<QPair<int, int> > ProcessImage::extendCoordX(QList<QPair<int, int> > &boundariesX)
{
    QPair<int, int> corBoundariesX;
    QList<QPair<int, int>> recBoundariesX;
    for (int i = 0; i < boundariesX.size(); i++){
        corBoundariesX.first = boundariesX[i].first + REC_X_LEFT_SHIFT;
        corBoundariesX.second = boundariesX[i].second + REC_X_RIGHT_SHIFT;
        recBoundariesX.push_back(corBoundariesX);
    }
    return recBoundariesX;
}

QList<QPair<int, int> > ProcessImage::getAllBoundaryY()
{
    QList<QPair<int,int>> listBoundaries;
    QPair<int /*upper*/, int /*down*/> boundary;

    for (int x = 0; x < mIMG_WIDTH; x++) {
        bool firstBoundary = true;
        int corrLen = 0;

        for (int y = 0; y < mIMG_HEIGHT; y++) {
            int y0 = x + y*mIMG_WIDTH;
            int y1 = y0 + mIMG_WIDTH;
            if (y1 >= mIMG_WIDTH * mIMG_HEIGHT) {
                break;
            }
            int color_y1 = (unsigned char)rawImg.at(y1);
            int color_y0 = (unsigned char)rawImg.at(y0);
            //go from black to white....
            if ((color_y1 - color_y0) > 0) {
                firstBoundary = false;
                boundary.first = y+1;
                corrLen = 0;
            }
            else if (firstBoundary == false && (color_y1 - color_y0) < 0) {
                //for from white to black
                firstBoundary = true;
                boundary.second = y;
                if (corrLen > CORR_LEN_Y){
                    listBoundaries.push_back(boundary);
                }
            }
            corrLen++;
        }
    }
    return listBoundaries;
}

/**
 * @brief ProcessImage::getBoundaryRecY
 * @param boundaries
 *
 * лучше всего передавать в функцию значение параметров для увеличения области поиска средней интенсивности
 * лазерного диода
 *
 * @return
 */
QPair<int/*y0*/,int/*y1*/> ProcessImage::getBoundaryRecY(const QList<QPair<int, int>>& boundaries)
{
    auto maxBoundary = boundaries[0];
    for (auto& boundary : boundaries) {
        if (maxBoundary.first > boundary.first)
            maxBoundary.first = boundary.first;

        if (maxBoundary.second < boundary.second)
            maxBoundary.second = boundary.second;
    }

    maxBoundary.first += REC_Y_UPPER_SHIFT;
    assert(maxBoundary.first > 0);
    maxBoundary.second += REC_Y_DOWN_SHIFT;
    assert(maxBoundary.second < mIMG_HEIGHT);

    return maxBoundary;
}

void ProcessImage::getCoorLaserDiodes()
{
    loadRawImg("D:\\Qt\\Projects\\f1_I30_t300_T25\\image_data[003040].bin8");
    removeColors();
    threshold();
    //drawHistogram();
#define GET_GREY_IMG
#ifdef GET_GREY_IMG
    saveRawImg("test.bmp");
#endif
    binRawImg();



    auto boundY = getAllBoundaryY();
    QPair<int, int> recY = getBoundaryRecY(boundY);


    auto boundX = getAllBoundaryX();
    auto prevRecX = getBoundaryRecX(boundX);
    QList<QPair<int, int> > corRecX = corBoundaryRecX(prevRecX);
    QList<QPair<int, int> > recX =  extendCoordX(corRecX);
    createXmlFile(recX, recY);

}



QVector<double> ProcessImage::getAverInten(const QString& fullFileName)
{
//    QVector<double> averInten;
    loadRawImg(fullFileName);
    if (rawImg.size() == 0){
        return averInten;
    }
    readXml();
    int sumInten = 0;
    int pixNum = 0;

    for (int count = 0; count < finalCoord.size(); count = count +2){
        sumInten = 0;
        pixNum = 0;
        for (int i = finalCoord[count].first; i < finalCoord[count+1].first; i++){
            for (int j = finalCoord[count].second; j < finalCoord[count+1].second; j++){
                if (rawImg[j * mIMG_WIDTH * 3 + i * 3] > 40){

                sumInten = sumInten + rawImg[j * mIMG_WIDTH * 3 + i * 3];
                pixNum++;}
            }
        }
        if (pixNum != 0){
            averInten.push_back(sumInten/pixNum);
        }
    }
    return averInten;
}

double ProcessImage::getWholeAverInten(const QString &fullFileName)
{
    wholeAverInten = 0;
    loadRawImg(fullFileName);
    if (rawImg.size() == 0){
        return -1;
    }

    for (int i = 0; i < rawImg.size(); i++){
        wholeAverInten += rawImg[i] - 15;
    }

    return wholeAverInten;
}

void ProcessImage::threshold()
{
    int color[256];
    memset(&color[0], 0x00, sizeof(int)*256);

    for (int i = 0; i < rawImg.size(); i++)
    {
            color[rawImg[i]]++;
    }
    QFile histfile("histogram.txt");
    histfile.open(QIODevice::WriteOnly);
    for (int i = 0; i < 256; i++){
        histfile.write(QString("%1\t%2\n").arg(i).arg(color[i]).toUtf8());
    }
    histfile.close();
}

// спросить реально ли это и надо ли??

//void ProcessImage::drawHistogram()
//{
//    FILE* gnuplotPipe = popen("gnuplot -persist","w");
//    if(!gnuplotPipe)
//            qDebug("there are no gnuplot");

//    string command =
//            "cd D:/Qt/Projects/build-diplom_processimage-Desktop_Qt_6_2_3_MinGW_64_bit-Debug"
//            "plot 'histogram.txt' u 1:2 w l";

//    fprintf(gnuplotPipe, "%s",command.c_str());
//    fflush(gnuplotPipe);
//    fprintf(gnuplotPipe, "exit\n");
//    pclose(gnuplotPipe);
//}


QDomDocument createXmlFile(QList<QPair<int, int>>& recX, QPair<int, int>& recY)
{
    QDomDocument doc("diodeCoordinates");
    QDomElement domElement = doc.createElement("diodeCoordinates");
    doc.appendChild(domElement);

    for (int i = 0; i < recX.size(); i++){
        QDomElement diode = createDiode(doc, recX[i], recY);
        domElement.appendChild(diode);
    }

    QFile file("diodeCoordinates.xml");
    if (file.open(QIODevice::WriteOnly)){
        QTextStream(&file) << doc.toString();
        file.close();
    }
    return doc;
}

QDomElement createDiode(QDomDocument& dom, QPair<int, int>& recX, QPair<int, int>& recY)
{
    static int number;
    QDomElement domElement = makeElement(dom, "diode", QString().setNum(number),"");
    domElement.appendChild(makeElement(dom, "firstBoundaryX", "", QString().setNum(recX.first)));
    domElement.appendChild(makeElement(dom, "firstBoundaryY", "", QString().setNum(recY.first)));
    domElement.appendChild(makeElement(dom, "secondBoundaryX", "", QString().setNum(recX.second)));
    domElement.appendChild(makeElement(dom, "secondBoundaryY", "", QString().setNum(recY.second)));
    number++;

    return domElement;

}

QDomElement makeElement(QDomDocument& dom, const QString& strName, const QString& strAttr, const QString& strText)
{
    QDomElement domElement = dom.createElement(strName);

    if (!strAttr.isEmpty()){
        QDomAttr domAttr = dom.createAttribute("number");
        domAttr.setValue(strAttr);
        domElement.setAttributeNode(domAttr);
    }

    if (!strText.isEmpty()){
        QDomText domText = dom.createTextNode(strText);
        domElement.appendChild(domText);
    }

    return domElement;

}

void ProcessImage::readXml()
{
    QDomDocument dom;
    QPair<int, int> coord;
    int* diodes = 0;
    QFile file ("diodeCoordinates.xml");
    if (file.open(QIODevice::ReadOnly)){
        if (dom.setContent(&file)){
            QDomElement domElement = dom.documentElement();
            diodes = traverseNode(domElement);

        }
    }
    for (int i = 0; i < 64; i = i+2){
        coord.first = diodes[i];
        coord.second = diodes[i+1];
        finalCoord.push_back(coord);

    }

}

int* ProcessImage::traverseNode(const QDomNode & node)
{
    static int diode[64] = {0};
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull()){
        if (domNode.isElement()){
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()){
                if (domElement.tagName() == "diode"){
                    int i = domElement.attribute("number", "").toInt();
                }
                else{
                    diode[counter] = domElement.text().toInt();
                    counter++;
                }
            }
        }

        traverseNode(domNode);

        domNode = domNode.nextSibling();

    }
    return diode;
}

ProcessImage::~ProcessImage()
{
    delete srcimg;
}
