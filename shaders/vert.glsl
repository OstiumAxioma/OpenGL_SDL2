#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

uniform mat4 scaleMatrix;

uniform vec2 offset;

out vec3 v_vertexColors; 					// TODO Uncomment 

void main()
{
    v_vertexColors = vertexColors;

    gl_Position = scaleMatrix * vec4(position.x + offset.x, position.y + offset.y, position.z, 1.0f);
}

