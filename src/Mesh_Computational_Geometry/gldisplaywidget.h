#ifndef GLDISPLAYWIDGET_H
#define GLDISPLAYWIDGET_H

#include <QGLWidget> // Module QtOpengL (classes QGLxxx in Qt4),
                     // with widget and graphics rendering classes descending from QGLWidget,
                     // deprecated and replaced by module QtGui (classes QOpenGLxxx )
                     // and module widgets (QWidget, QGraphicsView).
                     // UpgradeQt6: #include <QOpenGLWidget>
#include <QtWidgets>
#include <QTimer>
#include "mesh.h"  // Model

class GLDisplayWidget : public QGLWidget //UpgradeQt6: public QOpenGLWidget
{
public:
    explicit GLDisplayWidget(QWidget *parent = 0);

    void initializeGL(); // The scene may be initialized in this function since the GeometricWorld is a data member...
    void paintGL(); // Display the scene Gl
    void resizeGL(int width, int height);

    void zoom_in();
    void zoom_out();

    GeometricWorld _geomWorld; // The scene to be displayed
protected:
    // Mouse Management
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    QTimer _timer; // To update the scene
    float _X, _Y ,_Z; // Translation
    float _angle; // Rotation
    float _angle_x = 0.0f; // Rotation
    float _angle_y = 0.0f; // Rotation

    QPoint _lastPosMouse; // To keep the last position of the mouse
};

#endif // GLDISPLAYWIDGET_H
