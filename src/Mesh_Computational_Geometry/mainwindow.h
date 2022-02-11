#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_loadFileButton_released();
    void on_zoomInButton_released();
    void on_zoomOutButton_released();
    void on_colorButton_released();
    void on_colorSlider_valueChanged();

private:
    bool colorButton_faces = true;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
