#version 440

uniform mat4 viewMat;
uniform mat4 perspectiveMatrix;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 theNormal;

layout(location = 1) smooth out vec3 theNormalOut;
layout(location = 2) smooth out vec3 thePosOut;

void main()
{
	theNormalOut = normalize((theNormal/theNormal.w).xyz);
   thePosOut = position.xyz;
   gl_Position = perspectiveMatrix*viewMat*position;
}
