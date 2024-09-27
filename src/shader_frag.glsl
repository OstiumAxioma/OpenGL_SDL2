// shader_frag.glsl
#version 410 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0); // 使用插值后的颜色
}