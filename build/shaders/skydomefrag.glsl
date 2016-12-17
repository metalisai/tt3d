uniform vec3 v3LightDir;

const float g = -0.95;
const float g2 = 0.9025;

smooth in vec3 v3Direction;
smooth in vec3 RayleighColor;
smooth in vec3 MieColor;

void main (void)
{
	float cos = dot(v3LightDir, v3Direction) / length(v3Direction);
	float rayleighPhase = 0.75f * (1.0f + cos*cos);
	float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) *
					 (1.0f + cos*cos) / pow(1.0f + g2 - 2.0f * g * cos, 1.5f);
	gl_FragColor.rgb = rayleighPhase*RayleighColor + miePhase*MieColor;
	gl_FragColor.a = 1.0;
	gl_FragDepth = 1.0;
}
