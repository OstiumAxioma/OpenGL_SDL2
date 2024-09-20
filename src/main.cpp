//include libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
GLuint VBO, VAO, shaderProgram;

const std::vector<GLfloat> vertices = {
    -0.5f, -0.5f, 0.0f, // 左下角
    0.5f, -0.5f, 0.0f, // 右上角
    0.0f, 0.5f, 0.0f, // 右下角
};

void prepareTriangle(){

}
void prepareVAOForGLTriangle()
{
    //1. 准备顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下角
        0.5f, -0.5f, 0.0f, // 右上角
        0.0f, 0.5f, 0.0f, // 右下角
    };

    //2. 创建一个 VBO
    VBO = 0;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //3. 创建一个 VAO并绑定
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //4. 描述位置属性
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //5. 解绑 VAO
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
