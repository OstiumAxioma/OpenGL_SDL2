#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;


//VAO
GLuint gVertexArrayObject = 0;
//VBO
GLuint gVertexBufferObject = 0;

int shapeToDraw = 1;

//Program Object for the shaders
GLuint gGraphicsPipelineShaderProgram = 0;


//Read the fragment shader
std::string readShaderFile(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}

GLuint CompileShader(GLuint type, const std::string& source){
    GLuint shaderObject;
    if(type == GL_VERTEX_SHADER){
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }else if(type == GL_FRAGMENT_SHADER){
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }


    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);


    return shaderObject;


}

GLuint CreateShaderProgram(const std::string& vertexshadersource,
                        const std::string& fragmentshadersource){
    GLuint programObject = glCreateProgram();
    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);


    //validation
    glValidateProgram(programObject);
    return programObject;
}


void CreateGraphicsPipeline(){
    std::string gVertexShaderSource = readShaderFile("src/shader_vertex.glsl");
    std::string gFragmentShaderSource = readShaderFile("src/shader_frag.glsl");
    gGraphicsPipelineShaderProgram = CreateShaderProgram(
                                    gVertexShaderSource,
                                    gFragmentShaderSource);
}


void GetOpenGLVersionInfo(){
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void VertexSpecification(){
    std::vector<GLfloat> vertexData;
    if (shapeToDraw == 1) {
        vertexData = {
            // 位置               // 颜色
            -0.8f, -0.8f, 0.0f,   1.0f, 0.0f, 0.0f,  // 顶点1：红色
             0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,  // 顶点2：绿色
            -0.8f,  0.8f, 0.0f,   0.0f, 0.0f, 1.0f,  // 顶点3：蓝色
             0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,  // 顶点2：绿色（重复）
             0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f,  // 顶点4：黄色
            -0.8f,  0.8f, 0.0f,   0.0f, 0.0f, 1.0f   // 顶点3：蓝色（重复）
        };
    } else if (shapeToDraw == 2) {
        vertexData = {
            // 位置               // 颜色
            -0.8f, -0.8f, 0.0f,   1.0f, 0.0f, 0.0f,  // 顶点1：红色
             0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f,  // 顶点2：绿色
             0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f   // 顶点3：蓝色
        };
    }

    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // 生成 VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(GLfloat),
                 vertexData.data(),
                 GL_STATIC_DRAW);

    // 设置位置属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

    // 设置颜色属性
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout<<" SDL2 could not initialize video subsystem" << std::endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window",
                                    100,100,
                                    gScreenWidth, gScreenHeight,
                                    SDL_WINDOW_OPENGL);
    if(gGraphicsApplicationWindow == nullptr){
        std::cout << "SDL Window was not able to be created" << std::endl;
    }
    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
    if(gOpenGLContext == nullptr){
        std::cout << "OpenGL context not available" << std::endl;
        exit(1);
    }


    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cout << "Glad was not initialized" << std::endl;
        exit(1);
    }
    GetOpenGLVersionInfo();
}


void Input(){
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            std::cout << "Goodbye" << std::endl;
            gQuit = true;
        }
        switch (e.key.keysym.sym) {
            case SDLK_1:
                shapeToDraw = 1;
                std::cout << "1 key pressed" << std::endl;
                std::cout << "Drawing Square" << std::endl;
                break;
            case SDLK_2:
                shapeToDraw = 2;
                std::cout << "2 key pressed" << std::endl;
                std::cout << "Drawing Triangle" << std::endl;
                break;
            default:
                break;
        }
    }
}


void PreDraw(){
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0,0,gScreenWidth, gScreenHeight);
    glClearColor(1.f,1.f,0.f,1.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(gGraphicsPipelineShaderProgram);


}


void Draw(){
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    if (shapeToDraw == 1) {
        glDrawArrays(GL_TRIANGLES,0,6);
    } else if (shapeToDraw == 2) {
        glDrawArrays(GL_TRIANGLES,0,3);
    }
}


void MainLoop(){
    while(!gQuit){
        Input();


        PreDraw();


        Draw();


        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}


void CleanUp(){
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

#ifdef _WIN32
int WinMain(int argc, char *argv[])
{
#else
int main(int argc, char *argv[])
{
#endif
    InitializeProgram();

    VertexSpecification();

    CreateGraphicsPipeline();

    MainLoop();
    CleanUp();

    return 0;
}