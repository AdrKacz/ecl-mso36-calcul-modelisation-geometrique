#ifndef MESH_H
#define MESH_H

#include <QGLWidget> //UpgradeQt6: #include <QOpenGLWidget>

// TO MODIFY
class Point
{
public:
    double _x;
    double _y;
    double _z;

    Point():_x(),_y(),_z() {}
    Point(float x_, float y_, float z_):_x(x_),_y(y_),_z(z_) {}
};


//** TP : TO MODIFY

class Vertice
{
public:
    Point point;
    int index;

    Vertice();
    ~Vertice();
};

class Face
{
public:
    int vertice_indexes[3];

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
    Mesh(); // Constructors automatically called to initialize a Mesh (default strategy)
    ~Mesh(); // Destructor automatically called before a Mesh is destroyed (default strategy)
    //void drawMesh();
    //void drawMeshWireFrame();
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
