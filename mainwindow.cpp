
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "drawlines.h"
#include "loadimages.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    drLine = new DrawLines(ui->labImg);
    drLine->show();
    prImg = new ProcessImage;
    ldImg = new LoadImages;
    drLine->setGeometry(0, 0,
                        ui->labImg->width(), ui->labImg->height());

    connect(ldImg, &LoadImages::sig_FileForAVer,
            prImg, &ProcessImage::slot_FileForAver);

    connect(prImg, &ProcessImage::sig_SendAverInten,
            ldImg, &LoadImages::slot_WriteAverInten);

    connect(prImg, &ProcessImage::sig_SendAverWhole,
            ldImg, &LoadImages::slot_WriteWholeAverInten);
}

void MainWindow::openImgInWin()
{
    ui->labImg->setPixmap(QPixmap::fromImage(*prImg->srcimg, Qt::AutoColor));
}

void MainWindow::createVertTable(const QVector<int>& coorY)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(coorY.size());
    ui->tableWidget->setColumnCount(1);
    for (int i = 0; i < coorY.size(); i++){
        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setText(QString::number(coorY.at(i)));
        ui->tableWidget->setItem(i, 0, newItem);
    }
}


void MainWindow::on_pushButton_clicked()
{
    int countX = ui->lineEdit->text().toInt();
    int countY = ui->lineEdit_2->text().toInt();
    int shiftHor = ui->spinBox->text().toInt();
    int shiftVer = ui->spinBox_2->text().toInt();
    drLine->getCoorX(countX, shiftVer);
    drLine->getCoorY(countY, shiftHor);
    createVertTable(*drLine->coorY);
    drLine->drawLine();   
}

void MainWindow::on_action_triggered()
{
    QFileDialog fileDl;

#define RELEASE
#ifdef RELEASE
    fileName = fileDl.getOpenFileName(nullptr, "test", "D:/Qt/Projects");
    int size1 = fileName.size() - 1;
    while(fileName.at(size1--) != '/');

    curDir = fileName.left(size1 + 1);
    qDebug() << curDir;


#else
    fileName = "D:/Qt/Projects/I20A_F02H_t300_T20C_shift10120_goodLDI/image_data[001298].bin8";
#endif
    prImg->loadRawImg(fileName); // чтение файла bin8 в массив
    prImg->removeColors(); // удаление двух одинаковых значений пикселей из трех
    prImg->loadPixImg(); // преобразование bin8 в QImage
    prImg->saveRawImg("srcIMG.bmp");
    openImgInWin(); // вывод картинки в окно

}


MainWindow::~MainWindow()
{
    delete prImg;
    delete drLine;
    delete ui;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    static int prevVal = 0;
    int shiftHor = arg1 - prevVal;
    drLine->moveHorizontal(shiftHor);
    createVertTable(*drLine->coorY);
    prevVal = ui->spinBox->text().toInt();
}


void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    static int prevVal = 0;
    int shiftVer = arg1 - prevVal;
    drLine->moveVertical(shiftVer);
    prevVal = ui->spinBox_2->text().toInt();
}


void MainWindow::on_updateButton_clicked()
{
    int countY = ui->tableWidget->rowCount();

    for (int i = 0; i < countY; i++){
        QTableWidgetItem *newItem = ui->tableWidget->takeItem(i - 1, 1);
        drLine->coorY->operator[](i) = newItem->text().toInt();
    }
    drLine->drawLine();
    createVertTable(*drLine->coorY);
}


void MainWindow::on_enterButton_clicked()
{
    int yBottomNum = ui->lineEdit_3->text().toInt();
    int yTopNum = ui->lineEdit_4->text().toInt();

    int yBottom = drLine->coorX->at(yBottomNum);
    int yTop = drLine->coorX->at(yTopNum);

    QVector<int> horizBord;
    for (int i = 0; i < drLine->coorY->size(); i++){
        QTableWidgetItem *newItem = ui->tableWidget->takeItem(i - 1, 1);
        horizBord.append(newItem->text().toInt());
    }
    prImg->setCoorLaserDiodes(yBottom, yTop, horizBord);

}


void MainWindow::on_startButton_clicked()
{
    prImg->threshold = ui->spinBox_3->text().toInt();
    if (!curDir.size())
        ldImg->getFiles();
    else
        ldImg->getFiles(curDir);
}


void MainWindow::on_checkButton_clicked()
{
    if (drLine->coorY != nullptr) {
        delete drLine->coorY;
    }
    drLine->coorY = new QVector<int>();

    if (drLine->coorX != nullptr) {
        delete drLine->coorX;
    }
    drLine->coorX = new QVector<int>();

    QList<QPair<int, int>> coordList = prImg->readXml();

    drLine->coorX->append(coordList[0].second);
    drLine->coorX->append(coordList[1].second);
    drLine->coorY->append(coordList[0].first);
    for (int i = 1; i < coordList.size(); i+=2){
        drLine->coorY->append(coordList[i].first);
    }

    drLine->drawLine();
    createVertTable(*drLine->coorY);
}


void MainWindow::on_pushButton_2_clicked()
{
    prImg->threshold = ui->spinBox_3->text().toInt();
    prImg->paintImg("prcsIMG");
    ui->labImg->setPixmap(QPixmap::fromImage(*prImg->prcsimg, Qt::AutoColor));


}

