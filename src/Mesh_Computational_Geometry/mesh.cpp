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
    _mesh.faces[0].debug(0);
    _mesh.faces[34577].debug(34577);
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

void Mesh::verify_key(QString key, int f_index, int f_face) {
    QString f_info = QString("%1.%2").arg(f_index).arg(f_face);
    if (face_map_queue.find(key) == face_map_queue.end()) {
        face_map_queue[key] = f_info;
    } else {
        if (face_map_queue[key] == "Done") {
            qDebug() << QString("More than two faces at <%1>, problem with face <%2>, at point <%3>").arg(face_map_queue[key]).arg(f_index).arg(f_face);
        }
        // Set faces
        QString adj_f_info = face_map_queue[key];
        QStringList adj_info = adj_f_info.split(QLatin1Char('.'));
        int adj_f_index = adj_info[0].toInt();
        int adj_f_face = adj_info[1].toInt();
        faces[f_index].face_indexes[f_face] = adj_f_index;
        faces[adj_f_index].face_indexes[adj_f_face] = f_index;

        // found, should be changed only once, used to detect error in mesh
        // (could have erase instead, but no error detection)
        face_map_queue[key] = "Done";
    }
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
            int index = line_index - nb_vertices;

            face.vertice_indexes[0] = splitted_line[1].toInt();
            // Data structure - Store ONE Face in each Vertice
            // Data structure - Store ADJACENT Vertives in each Face
            if (!_mesh.vertices[face.vertice_indexes[0]].has_face) {
                _mesh.vertices[face.vertice_indexes[0]].has_face = true;
                _mesh.vertices[face.vertice_indexes[0]].face_index = index;
            }

            face.vertice_indexes[1] = splitted_line[2].toInt();
            if (!_mesh.vertices[face.vertice_indexes[1]].has_face) {
                _mesh.vertices[face.vertice_indexes[1]].has_face = true;
                _mesh.vertices[face.vertice_indexes[1]].face_index = index;
            }

            face.vertice_indexes[2] = splitted_line[3].toInt();
            if (!_mesh.vertices[face.vertice_indexes[2]].has_face) {
                _mesh.vertices[face.vertice_indexes[2]].has_face = true;
                _mesh.vertices[face.vertice_indexes[2]].face_index = index;
            }

            // Compute normal
            Point e_1 = _mesh.vertices[face.vertice_indexes[1]].point - _mesh.vertices[face.vertice_indexes[0]].point;
            Point e_2 = _mesh.vertices[face.vertice_indexes[2]].point - _mesh.vertices[face.vertice_indexes[0]].point;
            face.normal = e_1.cross(e_2).normalized();

            _mesh.faces.push_back(face);
            // Data structure - Store ADJACENT Faces in each Face
            for (int i = 0; i < 3; i++)
            {
                int i1 = (i + 1) % 3;
                int i2 = (i + 2) % 3;

                int f1 = face.vertice_indexes[i1];
                int f2 = face.vertice_indexes[i2];

                // Sort index to compare key between faces (relative order may change)
                if (f1 > f2) {
                    int fc = f2;
                    f2 = f1;
                    f1 = fc;
                }

                QString key = QString("%1.%2").arg(f1).arg(f2);
                _mesh.verify_key(key, index, i);
            }
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

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Point
// ===== ===== ===== ===== =====

QString Point::to_qstring() const
{
    return QString("(%1, %2, %3)").arg(_x).arg(_y).arg(_z);
}

Point Point::operator-(const Point& p) const
{
    return Point(_x - p._x, _y - p._y, _z - p._z);

}

Point Point::cross(const Point& p) {
    return Point(_y * p._z - _z * p._y, _z * p._x - _x * p._z, _x * p._y - _y * p._x);
}

double Point::norm2() const {
    return _x * _x + _y * _y + _z * _z;
}

double Point::norm() const {
    return sqrt(norm2());
}

Point Point::normalized() {
    double n = norm();
    return Point(_x / n, _y / n, _z / n);
}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Face
// ===== ===== ===== ===== =====

void Face::debug(int index)
{
    qDebug() << "---";
    qDebug() << QString("Face <%1>").arg(index);
    qDebug() << QString("Normal:   %1").arg(normal.to_qstring());
    qDebug() << QString("Vertices: (%1, %2, %3)").arg(vertice_indexes[0]).arg(vertice_indexes[1]).arg(vertice_indexes[2]);
    qDebug() << QString("Face:     (%4, %5, %6)").arg(face_indexes[0]).arg(face_indexes[1]).arg(face_indexes[2]);
    qDebug() << "---";
}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Mesh
// ===== ===== ===== ===== =====

void Mesh::compute_vertices_normal()
{

}

void Mesh::compute_vertices_laplacian()
{

}
