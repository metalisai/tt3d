#version 440

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

uniform vec3 camPos;
uniform vec4 lightDir;
uniform mat4 viewMat;
uniform sampler2D tex;
uniform int numberOfTilesX;

smooth in vec3 theNormal;
smooth in vec2 Texcoord;
smooth in vec3 thePos;

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

vec3 calculatePointLight(vec3 lightPos, vec4 lightColor, vec3 viewDir, float lightRadius)
{
	float lighting = 1.0;
	vec3 pointLightDir = lightPos-thePos;
	float lightDistance = length(pointLightDir);
	pointLightDir = normalize(pointLightDir);
	float attenuation = clamp((lightRadius-lightDistance)/lightRadius,0,1);

	lighting = max(dot(pointLightDir,theNormal),0.0);
	
	vec3 halfwayDir = normalize(pointLightDir + viewDir);
	float spec = pow(max(dot(theNormal, halfwayDir), 0.0), 32.0);	

	return lightColor.rgb*(lighting+spec)*attenuation*lightColor.a;
}

void main()
{
   vec3 specular = vec3(0.0,0.0,0.0);
   //vec3 fnormal = normalize(theNormal);

	vec3 diff = texture2D(tex, Texcoord).xyz;

	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	// data that doesn't change for different lights
	vec3 viewDir = normalize(camPos-thePos);

	// POINT LIGHTS
	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++)
	{
		int index = visibleLightIndicesBuffer.data[offset + i].index;
		vec4 lightpos = lightBuffer.data[index].position;
		vec4 lightcol = lightBuffer.data[index].color;
		float lightRadius = lightBuffer.data[index].paddingAndRadius.w;
		gl_FragColor.rgb += diff*calculatePointLight(lightpos.xyz, lightcol, viewDir, lightRadius);
	}

	// DIRECTIONAL LIGHT
	vec3 halfwayDir = normalize(lightDir.xyz + viewDir);
	float spec = pow(max(dot(theNormal, halfwayDir), 0.0), 32.0);	
	gl_FragColor.rgb += lightDir.w * diff * clamp(dot(lightDir.xyz, theNormal) + spec, 0.0, 1.0); 

   //gl_FragColor.rgb = normalize(camPos);
	gl_FragColor.a = 1.0;
}
