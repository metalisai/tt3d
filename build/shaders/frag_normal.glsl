#version 330

uniform sampler2D tex;
uniform sampler2D normalSampler;

uniform vec4 lightDir;

smooth in vec3 theNormal;
smooth in vec2 Texcoord;
smooth in vec3 thePos;
in mat3 TBN;

layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec3 outputNormal;
layout (location = 2) out vec3 outputPosition;

void main()
{
	vec3 finalNormal = texture(normalSampler, Texcoord).rgb;
	finalNormal = (finalNormal * 2.0) - 1.0;   
	//finalNormal = normalize(vec3(0.0,0.0,1.0));   
	finalNormal = normalize(TBN * finalNormal); 

   outputColor = texture(tex, Texcoord);
	outputColor.a = 1.0;
   outputNormal = finalNormal;
   outputPosition = thePos;
}
