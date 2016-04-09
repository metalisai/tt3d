#version 330

uniform sampler2D tex;
uniform vec4 lightDir;

smooth in vec3 theNormal;
smooth in vec2 Texcoord;
smooth in vec3 thePos;

layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec3 outputNormal;
layout (location = 2) out vec3 outputPosition;

void main()
{
	
   outputColor = texture(tex, Texcoord);
	outputColor.a = 1.0;
   outputNormal = normalize(theNormal);
   outputPosition = thePos;
}
