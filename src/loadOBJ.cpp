#include "loadOBJ.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool LoadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<Face>& faces)
{
    // 打开 OBJ 文件
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "无法打开 OBJ 文件: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        // 去除行首的空格
        line.erase(0, line.find_first_not_of(" \t"));
        // 忽略空行或注释行
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // 读取顶点坐标
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (prefix == "f") {
            // 读取面信息
            std::vector<int> indices;
            std::string vertexStr;
            while (iss >> vertexStr) {
                std::istringstream viss(vertexStr);
                std::string indexStr;
                // 读取 '/' 前的数字
                std::getline(viss, indexStr, '/');
                int index = std::stoi(indexStr);
                indices.push_back(index);
            }
            faces.emplace_back(indices);
        }
        // 忽略其他前缀的行
    }

    objFile.close();

    return true;
}