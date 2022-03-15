#include "mesh.h"

// DEBUG
#define FIXED_SIDES_TO_REMOVE 1

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
    _mesh.compute_vertices_normal();
    _mesh.compute_vertices_laplacian();
    _mesh.compute_faces_laplacian();
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

    // Easy peasy lemon squeezy, just define color for each vertexe
    glBegin(GL_TRIANGLES);
    glColor3d(0,0,1);
    glPointDraw(_bBox[0]);
    glColor3d(1,0,0);
    glPointDraw(_bBox[3]);
    glColor3d(0,1,0);
    glPointDraw(_bBox[1]);
    glEnd();
}

Point Mesh::laplacian_norm_to_color(double laplacian_norm)
{
    double r = 0., g = .5, b = 1.;
    double magnitude = laplacian_norm;
    magnitude = faces_norm_factor * (magnitude * faces_inv_difference_laplacian_norm + faces_constance_term_laplacian_norm);
    b -= std::min(1., magnitude);
    r += std::min(1., magnitude - 1.);
    return Point(r, g, b);
}

//void GeometricWorld::displaySide(unsigned int i) {
//    Side side = _mesh.sides[i];
//    unsigned int local_a = side.vertice_index_in_face_a;
//    unsigned int local_b = side.vertice_index_in_face_b;
//    glBegin(GL_LINES);
//    glPointDraw(_mesh.vertices[_mesh.faces[side.face_a_index].vertice_indexes[(local_a + 1) % 3]].point);
//    glPointDraw(_mesh.vertices[_mesh.faces[side.face_a_index].vertice_indexes[(local_a + 2) % 3]].point);
//    glPointDraw(_mesh.vertices[_mesh.faces[side.face_b_index].vertice_indexes[local_b]].point);
//    glEnd();
//}

void GeometricWorld::drawMesh() {
    if (_mesh.faces.size() <= 0) {
        return;
    }

    // From Sides

//    for (unsigned int i = 0 ; i  < _mesh.sides.size() ; i++) {
//        if (i == 241) {
//               glColor3d(0,0,0);
//        } else {
//              glColor3d(1,1,1);
//        }
//                       displaySide(i);
//    }

    for (unsigned int i = 0 ; i < _mesh.faces.size() ; i++) {
        if (!_mesh.faces[i].is_active) {
            continue;
        }
        // From Triangles
        glColor3d(1,1,1);
        for (unsigned int j = 0 ; j < 3 ; j++)
        {
            glBegin(GL_LINES);
            glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[j]].point);
            glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[(j + 1) % 3]].point);
            glEnd();
        }
            if (use_face_color) {
                Point color = _mesh.laplacian_norm_to_color(_mesh.faces[i].laplacian_norm);
                glColor3d(color._x, color._y, color._z);
                glBegin(GL_TRIANGLES);
                glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[0]].point);
                glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[1]].point);
                glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[2]].point);
                glEnd();
            } else {
                glBegin(GL_TRIANGLES);
                for (unsigned int j = 0 ; j < 3 ; j++)
                {
                    Point color = _mesh.laplacian_norm_to_color(_mesh.vertices[_mesh.faces[i].vertice_indexes[j]].laplacian_norm);
                    glColor3d(color._x, color._y, color._z);
                    glPointDraw(_mesh.vertices[_mesh.faces[i].vertice_indexes[j]].point);

                }
                glEnd();
            }
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

        // Add side (with the 2 faces informations
//        Side side = Side();
//        side.vertice_index_in_face_a = f_face;
//        side.face_a_index = f_index;
//        side.vertice_index_in_face_b = adj_f_face;
//        side.face_b_index = adj_f_index;
//        side.squared_length = (vertices[faces[f_index].vertice_indexes[(f_face + 1) % 3]].point - vertices[faces[f_index].vertice_indexes[(f_face + 2) % 3]].point).norm2();
//        sides.push_back(side);
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

            // Data structure - Store ONE Face in each Vertice
            face.vertice_indexes[0] = splitted_line[1].toInt();
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


void GeometricWorld::set_norm_factor(double d) {
    _mesh.faces_norm_factor = std::min(100., std::max(1., d));
}

void GeometricWorld::set_use_face_color(bool d) {
    use_face_color = d;
}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Point
// ===== ===== ===== ===== =====

QString Point::to_qstring() const
{
    return QString("(%1, %2, %3)").arg(_x).arg(_y).arg(_z);
}

Point Point::operator+(const Point& p) const
{
    return Point(_x + p._x, _y + p._y, _z + p._z);
}

Point Point::operator-(const Point& p) const
{
    return Point(_x - p._x, _y - p._y, _z - p._z);
}

Point Point::operator*(const double d) const
{
    return Point(_x * d, _y * d, _z * d);
}

Point Point::operator/(const double d) const
{
    return Point(_x / d, _y / d, _z / d);
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
    qDebug() << QString("Normal:      %1").arg(normal.to_qstring());
    qDebug() << QString("Vertices:    (%1, %2, %3)").arg(vertice_indexes[0]).arg(vertice_indexes[1]).arg(vertice_indexes[2]);
    qDebug() << QString("Face around: (%1, %2, %3)").arg(face_indexes[0]).arg(face_indexes[1]).arg(face_indexes[2]);
    qDebug() << "---";
}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Side
// ===== ===== ===== ===== =====

//bool Side::compare(const Side& a, const Side& b)
//{
//    return a.squared_length < b.squared_length;
//}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Mesh
// ===== ===== ===== ===== =====

void Mesh::compute_vertices_normal()
{
    // TODO: that can be done directly when reading the document
    // No need to "turn around", just store the number of adjacent faces and the cumulative normal
    qDebug() << QString("=== === === === ===");
    qDebug() << QString("Compute vertices normal, from 0 to %1").arg(vertices.size());
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        if (!vertices[i].is_active) {
            continue;
        }
        if (i != vertices[i].index)
        {
            qDebug() << QString("Error Index %1 instead of %2").arg(vertices[i].index).arg(i);
        }
        Point normal = Point();
        unsigned int n = 0;
        // Turn around vertice
        int initial_face_index = vertices[i].face_index;
        int current_face_index = initial_face_index;
        do {
            // Update normal
            n += 1;
            normal = normal + faces[current_face_index].normal;
            // Get next vertices in faces
            int next = 0;
            while (i != faces[current_face_index].vertice_indexes[next]) {
                next = (next + 1) % 3;
            }
            next = (next + 1) % 3;
            // Get face
            current_face_index = faces[current_face_index].face_indexes[next];
        } while (current_face_index != initial_face_index);

        // Set mean normal
        normal = normal / n;
        vertices[i].normal = normal;
    }
}

void Mesh::compute_faces_laplacian()
{
    // All values are between 0. and 6.
    faces_min_laplacian_norm = 6.;
    faces_max_laplacian_norm = 0.;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        Point laplacian = faces[i].normal * 3.;
        for (unsigned int j = 0; j < 3; j++)
        {
            laplacian = laplacian - faces[faces[i].face_indexes[j]].normal;
        }
        laplacian = laplacian / 3;
        faces[i].laplacian = laplacian;
        faces[i].laplacian_norm = laplacian.norm();
        faces_min_laplacian_norm = std::min(faces_min_laplacian_norm, faces[i].laplacian_norm);
        faces_max_laplacian_norm = std::max(faces_max_laplacian_norm, faces[i].laplacian_norm);
    }

    faces_inv_difference_laplacian_norm = 1 / (faces_max_laplacian_norm - faces_min_laplacian_norm);
    faces_constance_term_laplacian_norm = - faces_min_laplacian_norm * faces_inv_difference_laplacian_norm;
}

void Mesh::compute_vertices_laplacian()
{
    // All values are between 0. and 6.
    vertices_min_laplacian_norm = 6.;
    vertices_max_laplacian_norm = 0.;
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        Point laplacian = Point(0., 0., 0.);
        unsigned int n = 0;
        // Turn around vertice
        int initial_face_index = vertices[i].face_index;
        int current_face_index = initial_face_index;
        do {
            // Get next vertices in faces
            int next = 0;
            while (i != faces[current_face_index].vertice_indexes[next]) {
                next = (next + 1) % 3;
            }
            next = (next + 1) % 3;
            // Update laplacian (using next vertice in face -> adjacent)
            n += 1;
            // TODO: add cot, should not be only the difference
            laplacian = laplacian + (vertices[i].normal - vertices[faces[current_face_index].vertice_indexes[next]].normal) * 1;
            // Get face
            current_face_index = faces[current_face_index].face_indexes[next];
        } while (current_face_index != initial_face_index);

        // Mean
        laplacian = laplacian / n;

        vertices[i].laplacian = laplacian;
        vertices[i].laplacian_norm = laplacian.norm();
        vertices_min_laplacian_norm = std::min(vertices_min_laplacian_norm, vertices[i].laplacian_norm);
        vertices_max_laplacian_norm = std::max(vertices_max_laplacian_norm, vertices[i].laplacian_norm);
    }

    faces_inv_difference_laplacian_norm = 1 / (faces_max_laplacian_norm - faces_min_laplacian_norm);
    faces_constance_term_laplacian_norm = - faces_min_laplacian_norm * faces_inv_difference_laplacian_norm;
}

//void first_n_sides(const std::vector<Side>& s, unsigned int n)
//{
//    for (unsigned int i = 0; i < n; i++)
//    {
//        qDebug() << QString("<%1> [Between Face: <%2> - Face: <%3>] - Squared Length: <%4>").arg(i).arg(s[i].face_a_index).arg(s[i].face_b_index).arg(s[i].squared_length);
//    }
//}

void Mesh::reduce()
{
//    qDebug() << QString("Number of sides to sort: <%1>").arg(sides.size());
//    first_n_sides(sides, 10);
//    // Sort all sides
//    std::sort(sides.begin(), sides.end(), Side::compare);
//    qDebug() << QString("-Sides- is sorted.");
//    first_n_sides(sides, 10);

//    const unsigned int collapsed_edges = sides.size() * .1;
//    qDebug() << QString("Edge to collapse: %1").arg(collapsed_edges);
    for (unsigned int i = reduce_step * 100; i < (reduce_step + 1) * 100; i++)
    {
        collapse_edge();
    }
    reduce_step++;

    // Recompute adjacent (could have done only adjacent)
//    compute_vertices_normal();
//    compute_vertices_laplacian();
//    compute_faces_laplacian();
}

void Mesh::find_edge(unsigned int& face_index, unsigned int& vertices_local_index) {
    do {
        face_index = temp;
        vertices_local_index = 0;
        temp++;
    } while (!faces[face_index].is_active);
}

void Mesh::collapse_edge() {
    unsigned int index_face, local_index_vertices;
    find_edge(index_face, local_index_vertices);
    qDebug() << QString("F: <%1>, V: <%2>").arg(index_face).arg(local_index_vertices);

    // Faces to delete
    unsigned int index_face_up, index_face_down;
    // Adjacent Faces
    unsigned int index_face_up_a, index_face_up_b, index_face_down_a, index_face_down_b;
    // Edges vertices
    unsigned int index_vertices_a, index_vertices_b;
    // Local vertices
    unsigned int local_index_vertices_up_i, local_index_vertices_up_j, local_index_vertices_down_i, local_index_vertices_down_j;

    // Get index value UP
    index_face_up = index_face;
    unsigned int a = 0;
    local_index_vertices_up_i = (local_index_vertices + 1) % 3;
    local_index_vertices_up_j = (local_index_vertices + 2) % 3;
    index_face_up_a = faces[index_face_up].face_indexes[local_index_vertices_up_i];
    index_face_up_b = faces[index_face_up].face_indexes[local_index_vertices_up_j];
}

//void Mesh::collapse_edge(unsigned int side_index)
//{
//    // Note collapsed faces
//    unsigned int face_index_a = sides[side_index].face_a_index;
//    unsigned int face_index_b = sides[side_index].face_b_index;

//    // DEBUG (to find other way)
//    // Skip if one of the face already disabled
//    if (!faces[face_index_a].is_active || !faces[face_index_b].is_active) {
//        return;
//    }

//    // Adjacent faces of A
//    unsigned int face_index_a_plus = faces[face_index_a].face_indexes[(sides[side_index].vertice_index_in_face_a + 1) % 3];
//    unsigned int face_index_a_minus = faces[face_index_a].face_indexes[(sides[side_index].vertice_index_in_face_a + 2) % 3];

//    // Adjacent faces of B
//    unsigned int face_index_b_plus = faces[face_index_b].face_indexes[(sides[side_index].vertice_index_in_face_b + 1) % 3];
//    unsigned int face_index_b_minus = faces[face_index_b].face_indexes[(sides[side_index].vertice_index_in_face_b + 2) % 3];

//    // Define intersection vertice
//    unsigned int from_vertice_index = faces[sides[side_index].face_a_index].vertice_indexes[(sides[side_index].vertice_index_in_face_a + 1) % 3];
//    unsigned int to_vertice_index = faces[sides[side_index].face_a_index].vertice_indexes[(sides[side_index].vertice_index_in_face_a + 2) % 3];
//    vertices[from_vertice_index].is_active = false;
//    vertices[to_vertice_index].is_active = false;

//    Vertice intersect_vertice = Vertice();
//    intersect_vertice.point = (vertices[from_vertice_index].point + vertices[to_vertice_index].point) * .5;
//    intersect_vertice.index = vertices.size();
//    intersect_vertice.has_face = true;

//    // Desactive faces A and B
//    faces[face_index_a].is_active = false;
//    faces[face_index_b].is_active = false;

//    // --- Inform adjacent faces of each vertice side of new vertices
//    // From
//    int initial_face_index = vertices[from_vertice_index].face_index;
//    int current_face_index = initial_face_index;
//    do {
//        // Get root vertices in faces
//        int next = 0;
//        while (from_vertice_index != faces[current_face_index].vertice_indexes[next]) {
//            next = (next + 1) % 3;
//        }
//        // Change vertice in face
//        faces[current_face_index].vertice_indexes[next] = intersect_vertice.index;

//        // Get next vertice
//        next = (next + 1) % 3;

//        // Get face face
//        current_face_index = faces[current_face_index].face_indexes[next];
//    } while (current_face_index != initial_face_index);

//    // To
//    initial_face_index = vertices[to_vertice_index].face_index;
//    current_face_index = initial_face_index;

//    do {
//        // Get root vertices in faces
//        int next = 0;
//        while (to_vertice_index != faces[current_face_index].vertice_indexes[next]) {
//            next = (next + 1) % 3;
//        }
//        // Change vertice in face
//        faces[current_face_index].vertice_indexes[next] = intersect_vertice.index;

//        // Get next vertices
//        next = (next + 1) % 3;

//        // Get face face
//        current_face_index = faces[current_face_index].face_indexes[next];
//    } while (current_face_index != initial_face_index);

//    // Change up and down vertices
//    unsigned int not_used_index_face_a = 0;
//    while (faces[face_index_a].vertice_indexes[not_used_index_face_a] == from_vertice_index || faces[face_index_a].vertice_indexes[not_used_index_face_a] == to_vertice_index)
//    {
//        not_used_index_face_a += 1;
//    }
//    vertices[not_used_index_face_a].face_index = face_index_a_plus;

//    unsigned int not_used_index_face_b = 0;
//    while (faces[face_index_b].vertice_indexes[not_used_index_face_b] == from_vertice_index || faces[face_index_b].vertice_indexes[not_used_index_face_b] == to_vertice_index)
//    {
//        not_used_index_face_b += 1;
//    }
//    vertices[not_used_index_face_b].face_index = face_index_b_plus;

//    // --- Rewire

//    // Remake connections faces - a_plus with a_minus
//    unsigned int a_plus_to_a = 0;
//    unsigned int a_minus_to_a = 0;
//    while (faces[face_index_a_plus].face_indexes[a_plus_to_a] != face_index_a)
//    {
//        a_plus_to_a += 1;
//    }
//    while (faces[face_index_a_minus].face_indexes[a_minus_to_a] != face_index_a)
//    {
//        a_minus_to_a += 1;
//    }
//    faces[face_index_a_plus].face_indexes[a_plus_to_a] = face_index_a_minus;
//    faces[face_index_a_minus].face_indexes[a_minus_to_a] = face_index_a_plus;

//    // Remake connections faces - b_plus with b_minus
//    unsigned int b_plus_to_b = 0;
//    unsigned int b_minus_to_b = 0;
//    while (faces[face_index_b_plus].face_indexes[b_plus_to_b] != face_index_b)
//    {
//        b_plus_to_b += 1;
//    }
//    while (faces[face_index_b_minus].face_indexes[b_minus_to_b] != face_index_b)
//    {
//        b_minus_to_b += 1;
//    }
//    faces[face_index_b_plus].face_indexes[b_plus_to_b] = face_index_b_minus;
//    faces[face_index_b_minus].face_indexes[b_minus_to_b] = face_index_b_plus;

//    // Give a root to vertice and push it back
//    intersect_vertice.face_index = face_index_a_plus;
//    vertices.push_back(intersect_vertice);
//}


//    qDebug() << QString("=== === === === ===");
//    qDebug() << QString("=== === === === ===");
//    qDebug() << QString("From vertice <%1> to vertice <%2>").arg(from_vertice_index).arg(to_vertice_index);
//    qDebug() << QString("Between face (A) <%1> and face (B) <%2>").arg(face_index_a).arg(face_index_b);
//    qDebug() << QString("Face A connected to: %1, %2, %3").arg(faces[face_index_a].vertice_indexes[0]).arg(faces[face_index_a].vertice_indexes[1]).arg(faces[face_index_a].vertice_indexes[2]);
//    qDebug() << QString("Face B connected to: %1, %2, %3").arg(faces[face_index_b].vertice_indexes[0]).arg(faces[face_index_b].vertice_indexes[1]).arg(faces[face_index_b].vertice_indexes[2]);
//    qDebug() << QString("Face A Plus  <%1> connected to faces: %2, %3, %4").arg(face_index_a_plus).arg(faces[face_index_a_plus].face_indexes[0]).arg(faces[face_index_a_plus].face_indexes[1]).arg(faces[face_index_a_plus].face_indexes[2]);
//    qDebug() << QString("Face A Minus <%1> connected to faces: %2, %3, %4").arg(face_index_a_minus).arg(faces[face_index_a_minus].face_indexes[0]).arg(faces[face_index_a_minus].face_indexes[1]).arg(faces[face_index_a_minus].face_indexes[2]);
//    qDebug() << QString("Face B Plus  <%1> connected to faces: %2, %3, %4").arg(face_index_b_plus).arg(faces[face_index_b_plus].face_indexes[0]).arg(faces[face_index_b_plus].face_indexes[1]).arg(faces[face_index_b_plus].face_indexes[2]);
//    qDebug() << QString("Face B Minus <%1> connected to faces: %2, %3, %4").arg(face_index_b_minus).arg(faces[face_index_b_minus].face_indexes[0]).arg(faces[face_index_b_minus].face_indexes[1]).arg(faces[face_index_b_minus].face_indexes[2]);
//    qDebug() << QString("Face A Plus  <%1> connected to vertices: %2, %3, %4").arg(face_index_a_plus).arg(faces[face_index_a_plus].vertice_indexes[0]).arg(faces[face_index_a_plus].vertice_indexes[1]).arg(faces[face_index_a_plus].vertice_indexes[2]);
//    qDebug() << QString("Face A Minus <%1> connected to vertices: %2, %3, %4").arg(face_index_a_minus).arg(faces[face_index_a_minus].vertice_indexes[0]).arg(faces[face_index_a_minus].vertice_indexes[1]).arg(faces[face_index_a_minus].vertice_indexes[2]);
//    qDebug() << QString("Face B Plus  <%1> connected to vertices: %2, %3, %4").arg(face_index_b_plus).arg(faces[face_index_b_plus].vertice_indexes[0]).arg(faces[face_index_b_plus].vertice_indexes[1]).arg(faces[face_index_b_plus].vertice_indexes[2]);
//    qDebug() << QString("Face B Minus <%1> connected to vertices: %2, %3, %4").arg(face_index_b_minus).arg(faces[face_index_b_minus].vertice_indexes[0]).arg(faces[face_index_b_minus].vertice_indexes[1]).arg(faces[face_index_b_minus].vertice_indexes[2]);
//qDebug() << QString("=== === === === ===");
//qDebug() << QString("Face A Plus  <%1> connected to faces: %2, %3, %4").arg(face_index_a_plus).arg(faces[face_index_a_plus].face_indexes[0]).arg(faces[face_index_a_plus].face_indexes[1]).arg(faces[face_index_a_plus].face_indexes[2]);
//qDebug() << QString("Face A Minus <%1> connected to faces: %2, %3, %4").arg(face_index_a_minus).arg(faces[face_index_a_minus].face_indexes[0]).arg(faces[face_index_a_minus].face_indexes[1]).arg(faces[face_index_a_minus].face_indexes[2]);
//qDebug() << QString("Face B Plus  <%1> connected to faces: %2, %3, %4").arg(face_index_b_plus).arg(faces[face_index_b_plus].face_indexes[0]).arg(faces[face_index_b_plus].face_indexes[1]).arg(faces[face_index_b_plus].face_indexes[2]);
//qDebug() << QString("Face B Minus <%1> connected to faces: %2, %3, %4").arg(face_index_b_minus).arg(faces[face_index_b_minus].face_indexes[0]).arg(faces[face_index_b_minus].face_indexes[1]).arg(faces[face_index_b_minus].face_indexes[2]);
//qDebug() << QString("=== === === === ===");
