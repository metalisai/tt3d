#version 330

layout(location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 transformMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelMatrix;
uniform vec4 lightDir;

smooth out vec3 theNormal;
smooth out vec2 Texcoord;
smooth out vec3 thePos;

void main()
{
   mat4 MVP = perspectiveMatrix * transformMatrix;
   theNormal = normalize(mat3(modelMatrix)*normal);
   Texcoord = texCoord;
   gl_Position = (MVP*position);
   thePos = (modelMatrix*position).xyz;
}
