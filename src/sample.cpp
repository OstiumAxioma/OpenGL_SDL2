//include libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>

GLuint VBO, VAO, shaderProgram;
void prepareShader()
{
    //1. 建立 Shader 片段
    //定义顶点着色器 Vertex Shader
        const char* vertexShaderSource =
        "#version 410 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    //定义片元着色器 Fragment Shader
        const char* fragmentShaderSource =
        "#version 410 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    //2. 创建 Shader 程序
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    //3. 为 Shader 程序输入 Shader 代码
    //glShaderSource 函数用来为 Shader 对象上传 Shader 代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//"\0"结尾不需要告知字符串长度
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    //创建错误信息储存变量
    int success = 0;
    char infoLog[1024];
    //4. 编译 Shader 程序
    glCompileShader(vertexShader);
    //检查编译错误
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }

    glCompileShader(fragmentShader);
    //检查编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }

    //5. 建立 Shader 程序Shell
    shaderProgram = 0;
    shaderProgram = glCreateProgram();

    //6. 放入 Shader 编译结果
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    //7. 链接 Shader 程序成为可执行程序
    glLinkProgram(shaderProgram);
    //检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::LINK_FAILED\n"<<infoLog<<std::endl;
    }

    //8. 清理 Shader 程序
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
void prepareVAOForGLTriangle()
{
    //1. Prepare the vertices data
    const std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, 0.0f, // 左下角
        0.5f, -0.5f, 0.0f, // 右上角
        0.0f, 0.5f, 0.0f, // 右下角
    };

    //2. Create a VBO
    VBO = 0;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    //3. Create a VAO and bind the VBO to it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //4. Define the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //5. Unbind VAO
    glBindVertexArray(0);

}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT); //清理颜色缓冲区 GL_CALL是调试宏  

    //1. 绑定当前 program
    glUseProgram(shaderProgram);

    //2. 绑定 VAO
    glBindVertexArray(VAO);

    //3. 绘制图形
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //GL_CALL(glDrawArrays(GL_TRIANGLES, 2, 3));

}
#ifdef _WIN32
int WinMain(int argc, char *argv[])
{
#else
int main(int argc, char *argv[])
{
#endif
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create an SDL window
    SDL_Window *window = SDL_CreateWindow("SDL2 and GLAD Example",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Set the clear color to red (RGBA: 1.0, 0.0, 0.0, 1.0)
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

    // Prepare the shader
    //prepareShader();
    //prepareVAOForGLTriangle();

    // Main loop
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Clear screen with the red color
        glClear(GL_COLOR_BUFFER_BIT);
        //render();

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
