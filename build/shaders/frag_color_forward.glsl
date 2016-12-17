#version 440

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

uniform vec4 lightDir;
uniform sampler2D tex;
uniform vec3 camPos;
uniform int numberOfTilesX;

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
	float dotup = dot(theNormal,vec3(0.0f,1.0f,0.0f));
	//dotup = smoothstep(0.89, 0.9, dotup);
   	dotup*=dotup;
	vec3 color;
	if(thePos.y < 30.0)
		color = mix(vec3(0.4,0.7,0.1), vec3(0.2,0.2,0.07), smoothstep(0,30.0,thePos.y));
	else	
		color = mix(vec3(0.2,0.2,0.07), vec3(0.25,0.25,0.25), smoothstep(30.0,50.0,thePos.y));
	color = mix(vec3(0.25,0.25,0.25),color,dotup);

   float lighting = 1.0;

   vec3 specular = vec3(0.0,0.0,0.0);

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
		gl_FragColor.rgb += calculatePointLight(lightpos.xyz, lightcol, viewDir, lightRadius) * color;
	}

	// DIRECTIONAL LIGHT
	vec3 halfwayDir = normalize(lightDir.xyz + viewDir);
	//float spec = pow(max(dot(theNormal, halfwayDir), 0.0), 32.0);	
	float spec = 0.0;
	gl_FragColor.rgb += lightDir.w * color * clamp(dot(lightDir.xyz, theNormal) + spec, 0.0, 1.0);

	gl_FragColor.rgb += 0.25*color; // ambient

	gl_FragColor.a = 1.0;
}
