#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColors;

uniform mat4 scaleMatrix;
uniform mat4 offsetMatrix;
uniform mat4 rotationMatrix;

out vec3 v_vertexColors; 					// TODO Uncomment 

void main()
{
    v_vertexColors = vertexColors;

    gl_Position = scaleMatrix * offsetMatrix * rotationMatrix *vec4(position.x, position.y, position.z, 1.0f);
}

