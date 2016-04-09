#version 330

uniform sampler2D fbo_texture;
uniform sampler2D depth_texture;
uniform sampler2D normal_texture;
uniform sampler2D position_texture;

uniform vec4 lightDir;
uniform mat4 projMatrix;

in vec2 f_texcoord;

const float lightRange = 10;
const float lightRange2 = 100;

void main(void) {

  float mult = 1.0;
  float lighting = 1.0;

  vec3 specular = vec3(0.0,0.0,0.0);
  vec3 fnormal = normalize(texture2D(normal_texture,f_texcoord).xyz);
  vec3 pos = texture2D(position_texture, f_texcoord).xyz;


  vec3 pointLightDir = lightDir.xyz-pos;
  float lightDistance = length(pointLightDir);
  pointLightDir = normalize(pointLightDir);
  lighting = clamp(dot(fnormal,pointLightDir),0,1);
  float attenuation = 1.0 / (1.0 + 0.3*lightDistance + 0.11*lightDistance*lightDistance);
  //float lfade = clamp((lightRange-lightDistance)/lightRange,0,1);
  lighting *= attenuation;
  vec3 viewDir = normalize(-pos); // everything is in camera space, so camera position should always be 0?
  
  // blinn-phong
  vec3 halfwayDir = normalize(pointLightDir + viewDir);
  float spec = pow(max(dot(fnormal, halfwayDir), 0.0), 32.0);
  
  // phong
  //vec3 reflectDir = reflect(-pointLightDir, fnormal);
  //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

  specular = vec3(1.0,1.0,1.0) * spec; // light color here
  specular *= attenuation;

  vec4 diff = texture2D(fbo_texture, f_texcoord);

  gl_FragColor = diff*lighting*mult+diff*vec4(0.15,0.15,0.15,1.0)+vec4(specular,1.0);
  gl_FragDepth = texture2D(depth_texture, f_texcoord).x;
}
