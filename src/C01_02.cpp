#include <iostream>


struct Color {
    float r, g, b;
};

struct Pos {
    float x, y, z;
};

union Vertex {
    Color color;
    Pos pos;
};

union MyUnion
{
    int num;
    char ch[4];
};

int main()
{
    MyUnion u;
    u.num = 1;
    for (int i = 0; i < 4; i++)
    {
        //std::cout << "Byte " << i << ": " << (int)u.ch[i] << std::endl;
    }

    Vertex v;
    v.color.r = 1.0f;
    v.color.g = 0.0f;
    v.color.b = 0.0f;
    v.pos.x = 1.0f;
    v.pos.y = 2.0f;
    v.pos.z = 3.0f;

    std::cout << "Color: (" << v.color.r << ", " << v.color.g << ", " << v.color.b << ")" << std::endl;

    return 0;
}

// if we get:
// Byte 0: 1
// Byte 1: 0
// Byte 2: 0
// Byte 3: 0
// then the system is little-endian