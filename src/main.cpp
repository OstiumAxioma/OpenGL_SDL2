#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
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


//Program Object for the shaders
GLuint gGraphicsPipelineShaderProgram = 0;


//vertex shader
const std::string gVertexShaderSource =
    "#version 410 core\n"
    "in vec4 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
    "}\n";


//fragment shader
const std::string gFragmentShaderSource =
    "#version 410 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "   color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
    "}\n";

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
    const std::vector<GLfloat> vertexPosition{
        -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f
    };
   
   
    glGenVertexArrays(1,&gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);


    //generating the VBO
    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER,
                vertexPosition.size() * sizeof(GLfloat),
                vertexPosition.data(),
                GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);


}


void InitializeProgram(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout<<" SDL2 could not initialize video subsystem" << std::endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,4);
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
    glDrawArrays(GL_TRIANGLES,0,3);
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