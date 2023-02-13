#include <stdio.h>
#include <QtXml>
#include <QApplication>
#include <iostream>
#include <QString>
#include "processimage.h"



int diode[] = {0};


ProcessImage::ProcessImage()
{
    srcimg = new QImage(mIMG_WIDTH, mIMG_HEIGHT,QImage::Format_RGB888);
    prcsimg = nullptr;
}


void ProcessImage::loadRawImg(const QString &fileName)
{
    qDebug() << fileName;

    QFile tmpFile(fileName);
    tmpFile.open(QIODevice::ReadOnly);
    rawImg = tmpFile.readAll();
    tmpFile.close();
    qDebug() << rawImg.size();
}

void ProcessImage::loadPixImg()
{
    qDebug() << srcimg->width();

    for (int j = 0; j < mIMG_HEIGHT; j++){
        for(int i = 0; i < mIMG_WIDTH; i++){
            QColor pixCol((unsigned char)rawImg[j*mIMG_WIDTH+i],
                            (unsigned char)rawImg[j*mIMG_WIDTH+i],
                            (unsigned char)rawImg[j*mIMG_WIDTH+i]);
            srcimg->setPixelColor(i, j, pixCol);
        }
    }
}

void ProcessImage::removeColors()
{
    for (int i = 1; i < rawImg.size(); i++) {
        rawImg.remove(i, 2);
    }
    qDebug() << rawImg.size();
}

void ProcessImage::saveRawImg(const QString &fileName)
{

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

void ProcessImage::paintImg(const QString &filename)
{
    prcsimg = new QImage(mIMG_WIDTH, mIMG_HEIGHT, QImage::Format_RGB888);
    int i = 0;
    for (int y = 0; y < mIMG_HEIGHT; y++) {
        for (int x = 0; x < mIMG_WIDTH; x++) {
            if ((unsigned char)rawImg.at(i) >= threshold){
                prcsimg->setPixelColor(x, y, QColor(0x00, 0xff, 0x00));
                i++;
            }

            else{
                prcsimg->setPixelColor(x, y, QColor((unsigned char)rawImg.at(i),
                                                    (unsigned char)rawImg.at(i),
                                                    (unsigned char)rawImg.at(i)));
                i++;

            }
        }
    }
    bool res = prcsimg->save(filename + ".bmp");
    qDebug() << "bmp" << (res == true ? " saved" : " NOT saved");

}


QList<QPair<int, int> > ProcessImage::getBoundaryRecX(QVector<int> boundX)
{
    QList<QPair<int, int>> recX;
    QPair<int, int> boundaries;

    for (int i = 1; i < boundX.size(); i++){
        boundaries.first = boundX.at(i-1);
        boundaries.second = boundX.at(i);
        recX.append(boundaries);
    }

    return recX;

}


void ProcessImage::setCoorLaserDiodes(int yBottom, int yTop, QVector<int> boundX)
{
    QPair<int, int> recY;
    recY.second = yBottom;
    recY.first = yTop;
    QList<QPair<int, int>> recX = getBoundaryRecX(boundX);

    createXmlFile(recX, recY);

}



QVector<double> ProcessImage::getAverInten(const QString& fullFileName)
{
    QVector<double> averInten;
    averInten.clear();
    loadRawImg(fullFileName);
    if (rawImg.size() == 0){
        return averInten;
    }
    QList<QPair<int, int>> finalCoord = readXml();

    int sumInten = 0;
    int pixNum = 0;

    for (int count = 0; count < finalCoord.size(); count = count +2){
        sumInten = 0;
        pixNum = 0;
        for (int i = finalCoord[count].first; i < finalCoord[count+1].first; i++){
            for (int j = finalCoord[count].second; j < finalCoord[count+1].second; j++){
                if (rawImg[j * mIMG_WIDTH * 3 + i * 3] > threshold)
                {
                    sumInten = sumInten + rawImg[j * mIMG_WIDTH * 3 + i * 3];
                    pixNum++;
                }

            }
        }
        if (pixNum != 0){
            averInten.push_back(double(sumInten)/(double)pixNum);
        }
        else{
            averInten.push_back(0);
        }
    }
    return averInten;
}


double ProcessImage::getWholeAverInten(const QString &fullFileName)
{
    double wholeAver = 0;
    wholeAverInten = 0;
    loadRawImg(fullFileName);
    if (rawImg.size() == 0){
        return -1;
    }
    int countPx = 0;
    for (int i = 0; i < rawImg.size(); i += 3){
        if (rawImg[i] < threshold){
            continue;

        }
        wholeAverInten += rawImg[i];
        countPx++;
    }

    if (countPx == 0){
        wholeAver = 0;
    }
    else {

        wholeAver = wholeAverInten / countPx;
    }

    return wholeAver;
}


QDomDocument ProcessImage::createXmlFile(QList<QPair<int, int>>& recX, QPair<int, int>& recY)
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

QDomElement ProcessImage::createDiode(QDomDocument& dom, QPair<int, int>& recX, QPair<int, int>& recY)
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

QDomElement ProcessImage::makeElement(QDomDocument& dom, const QString& strName, const QString& strAttr, const QString& strText)
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

QList<QPair<int, int>> ProcessImage::readXml()
{
    QList<QPair<int, int>> finalCoord;
    QDomDocument dom;
    QPair<int, int> coord;
    int* diodes = nullptr;

    QFile file ("diodeCoordinates.xml");
    if (file.open(QIODevice::ReadOnly)){
        if (dom.setContent(&file)){
            QDomElement domElement = dom.documentElement();
            counter = 0;
            diodes = traverseNode(domElement);
        }
    }
    for (int i = 0; i < counter; i = i+2){
        coord.first = diodes[i];
        coord.second = diodes[i+1];
        finalCoord.push_back(coord);

    }
    return finalCoord;
}

void ProcessImage::slot_FileForAver(const QString& fileName)
{
    qDebug() << "s = " << fileName;
    QVector<double> temp = getAverInten(fileName);
    emit sig_SendAverInten(temp);

    emit sig_SendAverWhole(getWholeAverInten(fileName));
}

int* ProcessImage::traverseNode(const QDomNode & node)
{

    QDomNode domNode = node.firstChild();
    while (!domNode.isNull()){
        if (domNode.isElement()){
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()){
                if (domElement.tagName() == "diode"){
                    domElement.attribute("number", "").toInt();
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
    delete prcsimg;
}
