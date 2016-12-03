#version 440

uniform mat4 viewMat;
uniform mat4 perspectiveMatrix;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 theNormal;
layout(location = 2) in vec3 thePos;
//layout(location = 3) in vec2 Texcoord;

layout(location = 1) smooth out vec3 theNormalOut;
layout(location = 2) smooth out vec3 thePosOut;
//layout(location = 3) smooth out vec2 TexcoordOut;

void main()
{
	theNormalOut = theNormal;
   thePosOut = thePos;
 	//TexcoordOut = Texcoord;
   gl_Position = perspectiveMatrix*viewMat*position;
}
