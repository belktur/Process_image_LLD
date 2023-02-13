#include <QPainter>
#include "drawlines.h"
#define mIMG_WIDTH   720 // ширина
#define mIMG_HEIGHT   270 // высота

DrawLines::DrawLines(QWidget *parent) : QLabel(parent)
{
    coorY = nullptr;
    coorX = nullptr;
}

void DrawLines::getCoorX(int countX, int shift)
{
    if (coorX != nullptr) {
        delete coorX;
    }

    coorX = new QVector<int>();

    int step = mIMG_HEIGHT/(countX + 1);

    for (int i = 0; i < mIMG_HEIGHT; i += step){
        if (i != 0 ){
            coorX->append(i + shift);
        }
    }
}

void DrawLines::getCoorY(int countY,int shift)
{
    if (coorY != nullptr) {
        delete coorY;
    }

    coorY = new QVector<int>();

    int step = mIMG_WIDTH/(countY + 1);

    for (int i = 0; i < mIMG_WIDTH; i += step){
        if (i != 0 ){
            coorY->append(i + shift);
        }
    }
}

void DrawLines::drawLine()
{
    update();
}

void DrawLines::moveHorizontal(int shift)
{
    for (int i = 0; i < coorY->size(); i++){
        coorY->operator[](i) = coorY->at(i) + shift;
    }
    drawLine();
}

void DrawLines::moveVertical(int shift)
{
    for (int i = 0; i < coorX->size(); i++){
        coorX->operator[](i) = coorX->at(i) + shift;
    }
    drawLine();
}


void DrawLines::paintEvent(QPaintEvent *event)
{
    qDebug() << "paint event is issue";
    if ((coorY == nullptr)||(coorX == nullptr)) {
        return;
    }
    QPainter painter(this);
    painter.setPen(QPen(QColor(0xff, 0x00, 0x00)));

    for (int i = 0; i < coorY->size(); i++) {
         QString lineNumber = QString::number(i+1);
        painter.drawLine(QPoint(coorY->at(i), 0), QPoint(coorY->at(i), this->height()-1));
        painter.drawText(QPoint(coorY->at(i), 10),lineNumber);
    }

    QFont font = painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
    for (int i = 0; i < coorX->size(); i++) {
        QString lineNumber = QString::number(i);
        painter.drawLine(QPoint(0, coorX->at(i)), QPoint( this->width()-1, coorX->at(i)));
        painter.drawText(QPoint(5, coorX->at(i)),lineNumber);
    }
}

DrawLines::~DrawLines()
{
    delete coorX;
    delete coorY;

}
