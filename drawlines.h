#ifndef DRAWLINES_H
#define DRAWLINES_H

#include <QObject>
#include <QtWidgets>
#include <QPainter>


// класс для отрисовки и движения красных линий
class DrawLines : public QLabel
{
    Q_OBJECT
private:

public:
    QVector<int> *coorX;
    QVector<int> *coorY;
    explicit DrawLines(QWidget *parent = nullptr);
    void getCoorX(int countX, int shift);
    void getCoorY(int counY, int shift);
    void drawLine();
    void moveHorizontal(int shift);
    void moveVertical(int shift);

    ~DrawLines();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // DRAWLINES_H
