#include <iostream>

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
        std::cout << "Byte " << i << ": " << (int)u.ch[i] << std::endl;
    }

    return 0;
}

// if we get:
// Byte 0: 1
// Byte 1: 0
// Byte 2: 0
// Byte 3: 0
// then the system is little-endian