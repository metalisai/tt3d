#version 430

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

//uniform vec4 lightDir;
uniform mat4 viewMat;
uniform sampler2D tex;

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

const int numberOfTilesX = 64;

void main()
{
   float lighting = 1.0;

   vec3 specular = vec3(0.0,0.0,0.0);
   //vec3 fnormal = normalize(theNormal);

	vec3 diff = texture2D(tex, Texcoord).xyz;

	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++)
	{
		int index = visibleLightIndicesBuffer.data[offset + i].index;
		vec4 lightpos = viewMat*lightBuffer.data[index].position;
		vec4 lightcol = lightBuffer.data[index].color;
		float lightRadius = lightBuffer.data[index].paddingAndRadius.w;
		vec3 pointLightDir = lightpos.xyz-thePos;
		float lightDistance = length(pointLightDir);
		pointLightDir = normalize(pointLightDir);
		//float attenuation = 1.0 / clamp((1.0 + 0.3*lightDistance + 0.11*lightDistance*lightDistance),0.0001,99999.0);
		float attenuation = clamp((lightRadius-lightDistance)/lightRadius,0,1);

		lighting = max(dot(pointLightDir,theNormal),0.0);
      //lighting *= attenuation;
		
		vec3 viewDir = normalize(-thePos);
		vec3 halfwayDir = normalize(pointLightDir + viewDir);
   	float spec = pow(max(dot(theNormal, halfwayDir), 0.0), 32.0);	
		//vec3 reflectDir = reflect(-pointLightDir, theNormal);
   	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);	

		gl_FragColor.rgb += lightcol.rgb*(lighting*diff+spec*diff)*attenuation;
	}
	gl_FragColor.a = 1.0;
}
