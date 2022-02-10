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
    int index;
    bool has_face = false;
    int face_index;

    Point normal;

    Vertice();
    ~Vertice();
};

class Face
{
public:
    int vertice_indexes[3];
    int face_indexes[3];

    Point normal;

    void debug(int);

    Face();
    ~Face();
};

class Mesh
{
  // (Q ou STL)Vector of vertices
  // (Q ou STL)Vector of faces
  // Those who do not know about STL Vectors should have a look at cplusplus.com examples
public:
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
    void compute_vertices_laplacian();
};

class GeometricWorld //Here used to create a singleton instance
{
private:
  bool hasReadFirstLine = false;
  int nb_vertices = 0;
  int nb_faces = 0;
  int line_index = 0;
  QVector<Point> _bBox;  // Bounding box
  Mesh _mesh;
public :
  GeometricWorld();
  void draw();
  void drawWireFrame();
  void drawMesh();

  // input
  void loadFile();
  void startInput();
  void input(QString line);
  void endInput();
};


#endif // MESH_H
