#version 330

layout(location = 0) in vec4 position;

uniform mat4 transformMatrix;
uniform mat4 perspectiveMatrix;

void main()
{
   mat4 MVP = perspectiveMatrix * transformMatrix;
   gl_Position = (MVP*position);
}
