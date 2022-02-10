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
            // Read always in the same order -> 0, 1, 2
            Face face = Face();
            face.vertice_indexes[0] = splitted_line[1].toInt();
            // Data structure - Store ONE Face in each Vertice
            // Data structure - Store ADJACENT Vertives in each Face
            if (!_mesh.vertices[face.vertice_indexes[0]].has_face) {
                _mesh.vertices[face.vertice_indexes[0]].has_face = true;
                _mesh.vertices[face.vertice_indexes[0]].face_index = line_index - nb_vertices;
            }
            face.vertice_indexes[1] = splitted_line[2].toInt();
            if (!_mesh.vertices[face.vertice_indexes[1]].has_face) {
                _mesh.vertices[face.vertice_indexes[1]].has_face = true;
                _mesh.vertices[face.vertice_indexes[1]].face_index = line_index - nb_vertices;
            }
            face.vertice_indexes[2] = splitted_line[3].toInt();
            if (!_mesh.vertices[face.vertice_indexes[2]].has_face) {
                _mesh.vertices[face.vertice_indexes[2]].has_face = true;
                _mesh.vertices[face.vertice_indexes[2]].face_index = line_index - nb_vertices;
            }

            // Data structure - Store ADJACENT Faces in each Face
            // - (quick) Sort indexes to create key search
            int i0 = face.vertice_indexes[0];
            int i1 = face.vertice_indexes[1];
            int i2 = face.vertice_indexes[2];
            if (i0 > i1) {
                int ic = i1;
                i1 = i2;
                i2 = ic;
            }
            if (i2 < i0) {
                int ic = i2;
                i2 = i0;
                i0 = ic;

                ic = i2;
                i2 = i1;
                i1 = ic;
            } else if (i2 < i1) {
                int ic = i2;
                i2 = i1;
                i1 = ic;
            }
            QString key0 = QString("%1.%2").arg(i1).arg(i2);
            QString key1 = QString("%1.%2").arg(i0).arg(i2);
            QString key2 = QString("%1.%2").arg(i0).arg(i1);

            // ===== ===== =====
            // Debug
            if (line_index - nb_vertices < 2) {
                qDebug()  << QString("Faces: %1").arg(line_index - nb_vertices);
                qDebug()  << QString("<%1> - <%2> - <%3>").arg(i0).arg(i1).arg(i2);
                qDebug()  << QString("Key1: <%1>").arg(key0);
                qDebug()  << QString("Key2: <%1>").arg(key1);
                qDebug()  << QString("Key3: <%1>").arg(key2);
            }

            if (key0 == QString("8463.14979") || key1 == QString("8463.14979") || key2 == QString("8463.14979")) {
                qDebug()  << QString("Key <8463.14979> at: %1").arg(line_index - nb_vertices);
            }
            // ===== ===== =====
            if (_mesh.face_map_queue.find(key0) == _mesh.face_map_queue.end()) {
                _mesh.face_map_queue[key0] = line_index - nb_vertices;
            } else {
                if (_mesh.face_map_queue[key0] == -1) {
                    qDebug() << QString("More than two faces at <%1>, problem with face <%2>").arg(_mesh.face_map_queue[key0]).arg(line_index - nb_vertices);
                }
                // Set faces
//                face.face_indexes[0] = _mesh.face_map_queue[key0];

                // found, should be changed only once, used to detect error in mesh
                // (could have erase instead, but no error detection)
                _mesh.face_map_queue[key0] = -1;
            }

            if (_mesh.face_map_queue.find(key1) == _mesh.face_map_queue.end()) {
                _mesh.face_map_queue[key1] = line_index - nb_vertices;
            } else {
                if (_mesh.face_map_queue[key1] == -1) {
                    qDebug() << QString("More than two faces at <%1>, problem with face <%2>").arg(_mesh.face_map_queue[key1]).arg(line_index - nb_vertices);
                }
                // Set faces
//                face.face_indexes[1] = _mesh.face_map_queue[key1];

                // found, should be changed only once, used to detect error in mesh
                // (could have erase instead, but no error detection)
                _mesh.face_map_queue[key1] = -1;
            }

            if (_mesh.face_map_queue.find(key2) == _mesh.face_map_queue.end()) {
                _mesh.face_map_queue[key2] = line_index - nb_vertices;
            } else {
                if (_mesh.face_map_queue[key2] == -1) {
                    qDebug() << QString("More than two faces at <%1>, problem with face <%2>").arg(_mesh.face_map_queue[key2]).arg(line_index - nb_vertices);
                }
                // Set faces
//                face.face_indexes[2] = _mesh.face_map_queue[key2];

                // found, should be changed only once, used to detect error in mesh
                // (could have erase instead, but no error detection)
                _mesh.face_map_queue[key2] = -1;
            }

            _mesh.faces.push_back(face);
        } else {
            qDebug() << QString("Line %1 out of bound: <%2>").arg(line_index).arg(line);
        }
        line_index += 1;
    }
}

void GeometricWorld::endInput() {
    qDebug() << "End Input";
    qDebug() << nb_vertices;
    qDebug() << nb_faces;
}
