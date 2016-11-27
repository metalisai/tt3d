#version 330

uniform sampler2D fbo_texture;
uniform sampler2D depth_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec4 lightDir;
uniform mat4 projMatrix;

in vec2 f_texcoord;
in vec2 samples[4];

float LinearizeDepth(vec2 uv)
{
  float n = 1.1; // camera z near
  float f = 300.0; // camera z far
  float z = texture2D(depth_texture, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

vec4 raytrace(in vec3 reflectionVector, in float startDepth);

vec4 SSR()
{
	vec4 reflectedColor;

	vec3 normal = normalize( texture(normal_texture, f_texcoord) ).xyz;

	// Depth at current fragment
	float currDepth = LinearizeDepth(f_texcoord);

	// Eye position, camera is at (0, 0, 0), we look along negative z, add near plane to correct parallax
	vec3 eyePosition = normalize( vec3(0, 0, 0.1) ); // 0.1 - near plane
	vec4 reflectionVector = projMatrix * reflect( vec4(-eyePosition, 0), vec4(normal, 0) ) ;

	    // Call raytrace to get reflected color
	reflectedColor = raytrace(reflectionVector.xyz, currDepth);	


	return reflectedColor;
}

vec4 raytrace(in vec3 reflectionVector, in float startDepth)
{
	vec4 color = vec4(0.0f);
	float stepSize = 0.005; 
 
	float size = length(reflectionVector.xy);
	reflectionVector = normalize(reflectionVector/size);
	reflectionVector = reflectionVector * stepSize;
        
        // Current sampling position is at current fragment
	vec2 sampledPosition = f_texcoord;
        // Current depth at current fragment
	float currentDepth = startDepth;
        // The sampled depth at the current sampling position
        float sampledDepth = LinearizeDepth(sampledPosition);
	//float sampledDepth = linearizeDepth( texture(deferredDepthTex, sampledPosition).z );
 
	int its = 0;
        // Raytrace as long as in texture space of depth buffer (between 0 and 1)
	while(sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 &&
	      sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0 &&
              its < 20)
	{
                // Update sampling position by adding reflection vector's xy and y components
		sampledPosition = sampledPosition + reflectionVector.xy;
                // Updating depth values
		currentDepth = currentDepth + reflectionVector.z * startDepth;
		float sampledDepth = LinearizeDepth(sampledPosition);
                
                // If current depth is greater than sampled depth of depth buffer, intersection is found
		if(currentDepth > sampledDepth)
		{
                        // Delta is for stop the raytracing after the first intersection is found
                        // Not using delta will create "repeating artifacts"
			float delta = (currentDepth - sampledDepth);
			if(delta < 0.003f )
			{		
				float fade = 0.5-clamp(length(its*reflectionVector)*10,0,0.5);
				color = vec4(texture(fbo_texture, sampledPosition).rgb,fade);
				break;
			}
		}
		its++;
		stepSize *= 2;
	}
 
	return color;
}

void main(void) {

  float mult = 1.0;
  float lighting = 1.0;

  vec3 specular = vec3(0.0,0.0,0.0);
  vec3 fnormal = normalize(texture2D(normal_texture,f_texcoord).xyz);
  lighting = clamp(dot(fnormal,lightDir.xyz),0,1);
  vec3 pos = texture2D(position_texture, f_texcoord).xyz;
  vec3 pointLightDir = normalize(lightDir.xyz);

  vec3 viewDir = normalize(-pos); // everything is in camera space, so camera position should always be 0?
  
  // blinn-phong
  vec3 halfwayDir = normalize(pointLightDir + viewDir);
  float spec = pow(max(dot(fnormal, halfwayDir), 0.0), 32.0);
  
  // phong
  //vec3 reflectDir = reflect(-pointLightDir, fnormal);
  //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

  specular = vec3(1.0,1.0,1.0) * spec; // light color here

  //float texSamples[4];

  //texSamples[0] = LinearizeDepth(samples[0]);
  //texSamples[1] = LinearizeDepth(samples[1]);
  //texSamples[2] = LinearizeDepth(samples[2]);
  //texSamples[3] = LinearizeDepth(samples[3]);

  //mult *= float(abs(texSamples[0] - texSamples[1]) < 0.01);
  //mult *= float(abs(texSamples[2] - texSamples[3]) < 0.01);
  //mult *= float(length(texture2D(normal_texture, samples[0]).xyz-texture2D(normal_texture, samples[1]).xyz) < 0.45);
  //mult *= float(length(texture2D(normal_texture, samples[2]).xyz-texture2D(normal_texture, samples[3]).xyz) < 0.45);
  //gl_FragColor = vec4(fnormal,1.0);
  vec4 diff = texture2D(fbo_texture, f_texcoord);

  gl_FragColor = diff*lighting*mult+diff*vec4(0.15,0.15,0.15,1.0)+vec4(specular,1.0);
  gl_FragDepth = texture2D(depth_texture, f_texcoord).x;

  //vec4 ssrr = SSR();
  //gl_FragColor = mix(diff,ssrr,ssrr.a);
  //gl_FragColor = SSR();
}
