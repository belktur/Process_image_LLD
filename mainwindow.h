#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QDialog>
#include <QFileDialog>

#include "processimage.h"
#include "drawlines.h"
#include "loadimages.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    ProcessImage* prImg;
    DrawLines* drLine;
    LoadImages* ldImg;
    QString fileName;
    QString curDir;

public:
    MainWindow(QWidget *parent = nullptr);
    void openImgInWin();
    void createVertTable(const QVector<int>& coor);
    ~MainWindow();

protected:


signals:


private slots:

    void on_pushButton_clicked();

    void on_action_triggered();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_updateButton_clicked();

    void on_enterButton_clicked();

    void on_startButton_clicked();

    void on_checkButton_clicked();

    void on_pushButton_2_clicked();

public slots:

};
#endif // MAINWINDOW_H
