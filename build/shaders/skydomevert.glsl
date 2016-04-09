#version 330

uniform mat4 scaleMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 transformMatrix;

uniform vec3 v3CameraPos; //
uniform vec3 v3LightDir;	//	
uniform vec3 v3InvWavelength;	
uniform float fCameraHeight;	//
uniform float fCameraHeight2;	//
uniform float fOuterRadius;	//	
uniform float fOuterRadius2;	//
uniform float fInnerRadius;	//	
uniform float fInnerRadius2;	//
uniform float fKrESun;			//
uniform float fKmESun;			//
uniform float fKr4PI;			//
uniform float fKm4PI;		//
uniform float fScale;			
uniform float fScaleOverScaleDepth;//

const int nSamples = 10;
const float fSamples = 10.0;

smooth out vec3 v3Direction;
smooth out vec3 RayleighColor;
smooth out vec3 MieColor;

float scale(float fCos)
{
	float x = 1.0 - fCos;
	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(void)
{
	mat4 MVP = perspectiveMatrix * transformMatrix;

	vec3 v3Pos = (scaleMatrix*gl_Vertex).xyz;
	vec3 v3Ray = v3Pos - v3CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	vec3 v3Start = v3CameraPos;
	float fHeight = length(v3Start);
	float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	float fStartOffset = fDepth*scale(fStartAngle);

	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	MieColor.rgb = v3FrontColor * fKmESun;
	RayleighColor.rgb = v3FrontColor * (v3InvWavelength * fKrESun);
	
	gl_Position = MVP * gl_Vertex;
	v3Direction = v3CameraPos - v3Pos;
}
