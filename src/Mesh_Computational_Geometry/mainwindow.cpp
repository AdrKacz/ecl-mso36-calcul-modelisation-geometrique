#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mesh.h"  // Model

#include<QFile>
#include<QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadFileButton_released()
{
    return;
    GeometricWorld world = ui->widget->getGeomWorld();
    qDebug() << "Start Load File";
    // Select File - hard coded
    // Read File
    QFile inputFile("/Users/adrkacz/Desktop/GAM2022_Seance1/queen.off");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       qDebug() << "Open File";
       QTextStream in(&inputFile);
       world.startInput();
       while (!in.atEnd())
       {
          QString line = in.readLine();
          world.input(line);
       }
       world.endInput();
       inputFile.close();
    }
    qDebug() << "End Load File";
}

void MainWindow::on_zoomInButton_released() {
     ui->widget->zoomIn();
}

void MainWindow::on_zoomOutButton_released() {
     ui->widget->zoomOut();
}

