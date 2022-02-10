#include "mesh.h"

//#include<QDir>
//qDebug() << QDir::current();
#include<QDebug>
#include<QFile>

// Vertice

Vertice::Vertice() {}
Vertice::~Vertice() {}


// Face

Face::Face() {}
Face::~Face() {}

// Mesh

Mesh::Mesh() {}
Mesh::~Mesh() {}

// GeometricWorld

GeometricWorld::GeometricWorld()
{
    double width=1., depth=1., height=1.;
    _bBox.push_back(Point(0.0, 0.0, 0.0)); //0
    _bBox.push_back(Point(-1.0*width, 0.0, 0.0)); // 1
    _bBox.push_back(Point(0.0, -1.0*depth, 0.0)); // 2
    _bBox.push_back(Point(0.0, 0.0, 1.0*height)); // 3
    loadFile();
}

// The following functions could be displaced into a module OpenGLDisplayGeometricWorld that would include mesh.h

// Draw a Point
void glPointDraw(const Point & p) {
    glVertex3f(p._x, p._y, p._z);
}

//Example with a bBox
void GeometricWorld::draw() {
    glColor3d(1,0,0);
    glBegin(GL_TRIANGLES);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[1]);
    glPointDraw(_bBox[2]);
    glEnd();

    glColor3d(0,1,0);
    glBegin(GL_TRIANGLES);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[2]);
    glPointDraw(_bBox[3]);
    glEnd();

    glColor3d(0,0,1);
    glBegin(GL_TRIANGLES);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[3]);
    glPointDraw(_bBox[1]);
    glEnd();

    //glColor3d(1,1,0);
}

void GeometricWorld::drawMesh() {
    if (_mesh.faces.size() <= 0) {
        return;
    }
    glColor3d(1,1,1);
    for (unsigned int i = 0 ; i < _mesh.faces.size() ; i++) {
        glBegin(GL_TRIANGLES);
        glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[0]].point);
        glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[1]].point);
        glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[2]].point);
        glEnd();
    }
}

//Example with a wireframe bBox
void GeometricWorld::drawWireFrame() {
    glColor3d(0,1,0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[1]);
    glEnd();
    glColor3d(0,0,1);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[2]);
    glEnd();
    glColor3d(1,0,0);
    glBegin(GL_LINE_STRIP);
    glPointDraw(_bBox[0]);
    glPointDraw(_bBox[3]);
    glEnd();
}

// Input

void GeometricWorld::loadFile() {
    qDebug() << "Start Load File";
    // Select File - hard coded
    // Read File
    QFile inputFile("../../../../../meshes/queen.off");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       qDebug() << "Open File";
       QTextStream in(&inputFile);
       startInput();
       while (!in.atEnd())
       {
          QString line = in.readLine();
          input(line);
       }
       endInput();
       inputFile.close();
    }
    qDebug() << "End Load File";
}
void GeometricWorld::startInput() {
    qDebug() << "Start Input";
    hasReadFirstLine = false;
}

void GeometricWorld::input(QString line) {
    QStringList splitted_line = line.split(QLatin1Char(' '));

    if (!hasReadFirstLine) {
        // Read first line
        nb_vertices = splitted_line[0].toInt();
        nb_faces = splitted_line[1].toInt();
        hasReadFirstLine = true;
        line_index = 0;
    } else {
        // Read value
        if (line_index < nb_vertices) {
            Vertice vertice = Vertice();
            Point point = Point(
                        splitted_line[0].toFloat() * 10.0,
                        splitted_line[1].toFloat() * 10.0,
                        splitted_line[2].toFloat() * 10.0
                    );
            vertice.point = point;
            vertice.index = line_index;
            _mesh.vertices.push_back(vertice);
        } else if (line_index - nb_vertices < nb_faces) {
            Face face = Face();
            face.vertice_indexes[0] = splitted_line[1].toInt();
            face.vertice_indexes[1] = splitted_line[2].toInt();
            face.vertice_indexes[2] = splitted_line[3].toInt();
            _mesh.faces.push_back(face);
        } else {
            qDebug() << "Line out of bound";
            qDebug() << line_index;
            qDebug() << splitted_line;
        }
        line_index += 1;
    }
}

void GeometricWorld::endInput() {
    qDebug() << "End Input";
    qDebug() << nb_vertices;
    qDebug() << nb_faces;
}
