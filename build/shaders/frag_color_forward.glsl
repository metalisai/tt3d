#version 440

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

//uniform vec4 lightDir;
uniform sampler2D tex;
uniform vec4 lightDir;
uniform vec3 camPos;

layout(location = 1) smooth in vec3 theNormal;
layout(location = 2) smooth in vec3 thePos;
layout(location = 3) smooth in vec2 Texcoord;

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

const int numberOfTilesX = 64;

void main()
{
   float lighting = 1.0;

   vec3 specular = vec3(0.0,0.0,0.0);

	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * 1024;
	for (uint i = 0; i < 1024/16; i++)
	{
		uint index = i;
		vec4 lightpos = lightBuffer.data[index].position;
		vec4 lightcol = lightBuffer.data[index].color;
		float lightRadius = lightBuffer.data[index].paddingAndRadius.w;
		vec3 pointLightDir = lightpos.xyz-thePos;
		float lightDistance = length(pointLightDir);
		pointLightDir = normalize(pointLightDir);
		float attenuation = clamp((lightRadius-lightDistance)/lightRadius,0,1);

		lighting = max(dot(pointLightDir,theNormal),0.0);
		
		vec3 viewDir = normalize(camPos-thePos);
		//vec3 viewDir = normalize(-thePos);
		vec3 halfwayDir = normalize(pointLightDir + viewDir);
   	float spec = pow(max(dot(theNormal, halfwayDir), 0.0), 32.0);	

		gl_FragColor.rgb += lightcol.rgb*(lighting+spec)*attenuation*lightcol.a;
	}

	/*lighting = clamp(dot(theNormal,normalize(lightDir.xyz)),0,1);
	gl_FragColor.rgb += lighting*vec3(1.0);*/

	gl_FragColor.a = 1.0;
}
