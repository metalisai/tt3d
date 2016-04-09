#version 330 core
layout (location = 0) in vec4 vertex;

uniform mat4 transformMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
    gl_Position = perspectiveMatrix * transformMatrix * vertex;
}  
