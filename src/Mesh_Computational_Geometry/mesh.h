#ifndef MESH_H
#define MESH_H

#include <QGLWidget> //UpgradeQt6: #include <QOpenGLWidget>
#include<QString>
#include <unordered_map>

// TO MODIFY
class Point
{
public:
    double _x;
    double _y;
    double _z;

    Point():_x(),_y(),_z() {}
    Point(float x_, float y_, float z_):_x(x_),_y(y_),_z(z_) {}

    Point operator-(const Point&) const;
    Point operator+(const Point&) const;
    Point operator*(const double) const;
    Point operator/(const double) const;

    Point cross(const Point&);

    double norm() const;
    double norm2() const;
    Point normalized();

    QString to_qstring() const;
};


//** TP : TO MODIFY

class Vertice
{
public:
    Point point;
    unsigned int index;
    bool has_face = false;
    unsigned int face_index;

    bool is_active = true;

    Point normal;

    Point laplacian;
    double laplacian_norm;

    Vertice();
    ~Vertice();
};

class Face
{
public:
    unsigned int vertice_indexes[3];
    unsigned int face_indexes[3];
    unsigned int index;

    bool is_active = true;

    Point normal;

    Point laplacian;
    double laplacian_norm;

    void debug(int);
    unsigned int get_local_vertice_index(unsigned int);

    Face();
    ~Face();
};

class Mesh
{
  // (Q ou STL)Vector of vertices
  // (Q ou STL)Vector of faces
  // Those who do not know about STL Vectors should have a look at cplusplus.com examples
public:
    unsigned int reduce_step = 0;
    double faces_norm_factor = 1.;
    double vertices_norm_factor = 1.;

    double vertices_min_laplacian_norm;
    double vertices_max_laplacian_norm;

    double faces_min_laplacian_norm;
    double faces_max_laplacian_norm;
    double faces_inv_difference_laplacian_norm;
    double faces_constance_term_laplacian_norm;

//    std::vector<Side> sides;
    std::vector<Vertice> vertices;
    std::vector<Face> faces;
    std::unordered_map<QString, QString> face_map_queue;
    Mesh(); // Constructors automatically called to initialize a Mesh (default strategy)
    ~Mesh(); // Destructor automatically called before a Mesh is destroyed (default strategy)
    //void drawMesh();
    //void drawMeshWireFrame();

    // Create mesh Data Structure Helper
    void verify_key(QString, int, int);
    // Compute normal and variation
    void compute_vertices_normal();
    void compute_faces_laplacian();
    void compute_vertices_laplacian();

    Point laplacian_norm_to_color(double);

    // Space reduction
    void reduce();
    void find_edge(unsigned int&, unsigned int&);
    unsigned int temp = 0;
    void collapse_edge();
};

class GeometricWorld //Here used to create a singleton instance
{
private:
//  void displaySide(unsigned int);
  bool use_face_color = true;

  bool hasReadFirstLine = false;
  int nb_vertices = 0;
  int nb_faces = 0;
  int line_index = 0;
  QVector<Point> _bBox;  // Bounding box
public :
  Mesh _mesh;

  GeometricWorld();
  void draw();
  void drawWireFrame();
  void drawMesh();

  // input
  void loadFile();
  void startInput();
  void input(QString line);
  void endInput();

    void set_norm_factor(double);
    void set_use_face_color(bool);
//  void set_faces_norm_factor(double);
//  void set_vertices_norm_factor(double);
};


#endif // MESH_H
