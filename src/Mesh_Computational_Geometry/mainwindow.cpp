#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mesh.h"  // Model

#include<QFile>
#include<QDebug>
#include<QString>

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
//    qDebug() << "Start Load File";
//    // Select File - hard coded
//    // Read File
//    QFile inputFile("/Users/adrkacz/Desktop/GAM2022_Seance1/queen.off");
//    if (inputFile.open(QIODevice::ReadOnly))
//    {
//       qDebug() << "Open File";
//       QTextStream in(&inputFile);
//       world.startInput();
//       while (!in.atEnd())
//       {
//          QString line = in.readLine();
//          world.input(line);
//       }
//       world.endInput();
//       inputFile.close();
//    }
//    qDebug() << "End Load File";
}

void MainWindow::on_zoomInButton_released() {
     ui->widget->zoom_in();
}

void MainWindow::on_zoomOutButton_released() {
     ui->widget->zoom_out();
}

void MainWindow::on_colorButton_released() {
    if (colorButton_faces) {
        ui->colorButton->setText("Faces");
    } else {
        ui->colorButton->setText("Vertices");
    }
    colorButton_faces = !colorButton_faces;
    ui->widget->_geomWorld.set_use_face_color(colorButton_faces);
}

void MainWindow::on_reduceMesh_released() {
    ui->widget->_geomWorld._mesh.reduce();
}

void MainWindow::on_colorSlider_valueChanged() {
    ui->widget->_geomWorld.set_norm_factor(ui->colorSlider->value());
//    ui->widget->set_norm_factor(ui->colorSlider->value());
}

