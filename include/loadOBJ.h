#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <vector>
#include <string>

// 顶点类
class Vertex {
public:
    float x, y, z;

    Vertex(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_) {}
};

// 面类
class Face {
public:
    std::vector<int> vertexIndices;

    Face(const std::vector<int>& indices)
        : vertexIndices(indices) {}
};

// 声明加载 OBJ 文件的函数
bool LoadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<Face>& faces);

#endif // LOADOBJ_H