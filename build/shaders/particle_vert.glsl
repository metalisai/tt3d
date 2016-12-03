#version 440
layout (location = 0) in vec4 vertex;

uniform mat4 transformMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
   gl_Position = vertex;
}  
