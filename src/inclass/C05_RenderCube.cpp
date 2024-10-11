#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// 包含 GLM 库头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 全局变量
GLuint gGraphicsPipelineShaderProgram;

// 函数声明
void CreateGraphicsPipeline();
std::string LoadShaderAsString(const std::string& filename);
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

int main(int argc, char* argv[])
{
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 设置 OpenGL 版本和属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow("Rotating Cube with SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        std::cout << "创建 SDL 窗口失败: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 创建 OpenGL 上下文
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        std::cout << "创建 OpenGL 上下文失败: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "初始化 GLAD 失败" << std::endl;
        return -1;
    }

    // 设置视口
    glViewport(0, 0, 800, 600);

    // 创建图形管线
    CreateGraphicsPipeline();

    // 设置顶点数据
    float vertices[] = {
        // 立方体的顶点坐标（仅8个唯一顶点）
        -0.5f, -0.5f, -0.5f, // 0 左下后
         0.5f, -0.5f, -0.5f, // 1 右下后
         0.5f,  0.5f, -0.5f, // 2 右上后
        -0.5f,  0.5f, -0.5f, // 3 左上后
        -0.5f, -0.5f,  0.5f, // 4 左下前
         0.5f, -0.5f,  0.5f, // 5 右下前
         0.5f,  0.5f,  0.5f, // 6 右上前
        -0.5f,  0.5f,  0.5f  // 7 左上前
    };

    // 索引数组
    unsigned int indices[] = {
        // 后面
        0, 1, 2,
        2, 3, 0,
        // 前面
        4, 5, 6,
        6, 7, 4,
        // 左面
        0, 3, 7,
        7, 4, 0,
        // 右面
        1, 5, 6,
        6, 2, 1,
        // 底面
        0, 1, 5,
        5, 4, 0,
        // 顶面
        3, 2, 6,
        6, 7, 3
    };

    // 创建 VAO、VBO 和 EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);  
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定 VAO
    glBindVertexArray(VAO);

    // 绑定 VBO 并传输顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定 EBO 并传输索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 主循环
    bool running = true;
    SDL_Event event;
    while (running)
    {
        // 事件处理
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }
            // 如果需要处理窗口尺寸变化，可以在这里添加代码
        }

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置背景颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲

        // 激活着色器程序
        glUseProgram(gGraphicsPipelineShaderProgram);

        // 创建变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        float timeValue = SDL_GetTicks() / 1000.0f; // SDL_GetTicks 返回毫秒，需要转换为秒
        model = glm::rotate(model, timeValue * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 
                        800.0f / 600.0f, 0.1f, 100.0f);

        // 传递矩阵到着色器
        int modelLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "model");
        int viewLoc  = glGetUniformLocation(gGraphicsPipelineShaderProgram, "view");
        int projLoc  = glGetUniformLocation(gGraphicsPipelineShaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 绘制立方体
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 交换缓冲区
        SDL_GL_SwapWindow(window);
    }

    // 释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(gGraphicsPipelineShaderProgram);

    // 销毁 OpenGL 上下文和窗口
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

/**
 * 从文件中加载着色器源码
 *
 * @param filename 着色器文件的路径
 * @return 返回着色器源码字符串
 */
std::string LoadShaderAsString(const std::string& filename)
{
    std::ifstream shaderFile(filename);
    if (!shaderFile.is_open())
    {
        std::cout << "无法打开着色器文件: " << filename << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

/**
 * 创建着色器程序
 *
 * @param vertexShaderSource 顶点着色器源码
 * @param fragmentShaderSource 片段着色器源码
 * @return 返回创建的着色器程序的 ID
 */
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
    // 创建顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vShaderCode = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "顶点着色器编译失败:\n" << infoLog << std::endl;
    }

    // 创建片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fShaderCode = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    // 检查编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "片段着色器编译失败:\n" << infoLog << std::endl;
    }

    // 链接着色器程序
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "着色器程序链接失败:\n" << infoLog << std::endl;
    }

    // 删除着色器，因为它们已经链接到程序中了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/**
 * 创建图形管线
 */
void CreateGraphicsPipeline()
{
    std::string vertexShaderSource   = LoadShaderAsString("../shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("../shaders/frag.glsl");

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
    {
        std::cout << "着色器加载失败，程序将退出。" << std::endl;
        exit(-1);
    }

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}