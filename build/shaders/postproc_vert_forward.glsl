#version 330

layout(location = 0) in vec2 v_coord;

out vec2 f_texcoord;
out vec2 samples[4];

void main(void) {
  gl_Position = vec4(v_coord, 0.0, 1.0);
  vec2 texCoord = (v_coord + 1.0) / 2.0;
  f_texcoord = texCoord;
}
