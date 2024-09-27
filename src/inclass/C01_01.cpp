#include <iostream>
using namespace std;

struct MyStruct
{
    char x; //4 bytes
    //memory are assigned in unit of 4 bytes in cpp, even we assigned 1 byte data will still take 4 bytes
    int r, g, b; //4 bytes each
};

union MyUnion 
{
    char x;
    int r[10];
    int g[5];
    int b[7];
};

int main()
{
    MyStruct color1;
    color1.r = 255;
    color1.g = 0;
    color1.b = 0;

    MyUnion color2;
    color2.r[0] = 255;
    // union will take the memory of the largest data type in it, in this case int r, so it will take 40 bytes

    long long int a;
    cout << "Size of MyStruct: " << sizeof(color1) << endl;
    cout << "Size of MyUnion: " << sizeof(color2) << endl;

    return 0;
}