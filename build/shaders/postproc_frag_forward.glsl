#version 330

uniform sampler2D fbo_texture;
uniform sampler2D depth_texture;

in vec2 f_texcoord;

float LinearizeDepth(vec2 uv)
{
  float n = 1.1; // camera z near
  float f = 300.0; // camera z far
  float z = texture2D(depth_texture, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main(void) {
  vec4 diff = texture2D(fbo_texture, f_texcoord);
  gl_FragColor = diff;
  gl_FragDepth = texture2D(depth_texture, f_texcoord).x;
}
