#version 330

uniform vec4 lightDir;
uniform mat4 transformMatrix;

//smooth in vec3 theColor;
smooth in vec3 theNormal;

out vec4 outputColor;

void main()
{
   vec3 normalCS = normalize(mat3(transformMatrix)*normalize(theNormal));
   float cosTheta = clamp(dot(normalCS,lightDir.xyz),0,1);
   outputColor = vec4(vec3(0.9,0.9,0.9)*cosTheta,1.0);
}
