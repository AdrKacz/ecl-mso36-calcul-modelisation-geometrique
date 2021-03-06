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

void GeometricWorld::drawMesh() {
    if (_mesh.faces.size() <= 0) {
        return;
    }
    unsigned int count = 0;
    for (unsigned int i = 0 ; i < _mesh.faces.size() ; i++) {
        if (!_mesh.faces[i].is_active) {
            continue;
        }
        count++;
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
    qDebug() << QString("Number of face: %1").arg(count);
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
            face.index = index;

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

unsigned int Face::get_local_vertice_index(unsigned int global_vertice_index) {
    for (unsigned int i = 0 ; i < 3 ; i++) {
        if (vertice_indexes[i] == global_vertice_index) {
            return i;
        }
    }
     qDebug() << QString("ERROR: Vertices <%1> not found in face <%2>: (%3, %4, %5)")
                .arg(global_vertice_index)
                .arg(index)
                .arg(vertice_indexes[0])
                .arg(vertice_indexes[1])
                .arg(vertice_indexes[2]);
    return -1;
}

// ===== ===== ===== ===== =====
// ===== ===== ===== ===== ===== Mesh
// ===== ===== ===== ===== =====

void Mesh::compute_vertices_normal()
{
    // NOTE: that can be done directly when reading the document
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

void Mesh::reduce()
{
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
        while (vertices_local_index < 3 && !vertices[faces[face_index].vertice_indexes[vertices_local_index]].is_active) {
            vertices_local_index++;
        }
        temp++;
    } while (!faces[face_index].is_active && vertices_local_index < 3);
}

unsigned int Mesh::get_id_local_sommet(unsigned int id_face, unsigned int id_sommet){
    //on trouve l'id local
    int i;
    for(i = 0 ; i < 3; i++){
        if(faces[id_face].vertice_indexes[i] == id_sommet)
            break;
    }
    return i;
}

void Mesh::find_shortest_edge(unsigned int& face_id, unsigned int& id_sommet_oppose){
    float min_length = 10000;
    for(unsigned int i = 0; i < faces.size() ; i++){
        if(faces[i].is_active)
        for(unsigned int j = 0; j < 3; j++){
            float edge_length = (vertices[faces[i].vertice_indexes[j]].point - vertices[faces[i].vertice_indexes[(j + 1) % 3]].point).norm();
            if(edge_length < min_length){
                face_id = i;
                id_sommet_oppose = (j + 2) % 3;
                min_length = edge_length;
            }
        }
        if(min_length < 0.001){
            break;
        }
    }
}

void Mesh::collapse_edge(){
    unsigned int id_face, id_local_sommet_oppose;
    find_shortest_edge(id_face, id_local_sommet_oppose);

    unsigned int index_face_down, index_face_up;
    unsigned int index_face_down_right, index_face_down_left, index_face_up_right, index_face_up_left;
    unsigned int index_vertices_a, index_vertices_b;
    unsigned int index_vertice_down_right, index_vertice_down_left, index_vertice_up_right, index_vertice_up_left;

    // Up
    index_face_up = id_face;
    index_vertice_up_right = (id_local_sommet_oppose + 1) % 3;
    index_vertice_up_left = (id_local_sommet_oppose + 2) % 3;
    index_face_up_right = faces[index_face_up].face_indexes[index_vertice_up_left];
    index_face_up_left = faces[index_face_up].face_indexes[index_vertice_up_right];

    // Vertices
    index_vertices_a = faces[index_face_up].vertice_indexes[index_vertice_up_right];
    index_vertices_b = faces[index_face_up].vertice_indexes[index_vertice_up_left];

    // Down
    index_face_down = faces[index_face_up].face_indexes[id_local_sommet_oppose];
    index_vertice_down_right = get_id_local_sommet(index_face_down, index_vertices_a);
    index_vertice_down_left = get_id_local_sommet(index_face_down, index_vertices_b);
    index_face_down_right = faces[index_face_down].face_indexes[index_vertice_down_left];
    index_face_down_left = faces[index_face_down].face_indexes[index_vertice_down_right];

    // Rewire adjacents faces
    faces[index_face_up_right].face_indexes[(get_id_local_sommet(index_face_up_right, index_vertices_a) + 2) % 3] = index_face_up_left;
    faces[index_face_up_left].face_indexes[(get_id_local_sommet(index_face_up_left, index_vertices_b) + 1) % 3] = index_face_up_right;
    faces[index_face_down_right].face_indexes[(get_id_local_sommet(index_face_down_right, index_vertices_a) + 1) % 3] = index_face_down_left;
    faces[index_face_down_left].face_indexes[(get_id_local_sommet(index_face_down_left, index_vertices_b) + 2) % 3] = index_face_down_right;


    // Rewire to correct ertices
    unsigned int current_face = index_face_down_left;
    unsigned int id_local_sommet_s2;
    int i = 0;
    do{
        i++;
        if(i > 32){
            break;
        }
        id_local_sommet_s2 = get_id_local_sommet(current_face,index_vertices_b);
        faces[current_face].vertice_indexes[id_local_sommet_s2] = index_vertices_a;

        current_face = faces[current_face].face_indexes[(id_local_sommet_s2+1)%3];

    }while(current_face != index_face_up_right);

    // Move vertice rewired
    vertices[index_vertices_a].point = (vertices[index_vertices_a].point + vertices[index_vertices_b].point)/2;

    // Deactive unused resources
    faces[index_face_up].is_active = false;
    faces[index_face_down].is_active = false;
    vertices[index_vertices_b].is_active = false;

}
