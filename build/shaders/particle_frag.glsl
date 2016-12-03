#version 440

//uniform isampler2D tex;
uniform isampler1D mcubesLookup2;
 
void main(void)
{
	//float test = float(lookupBuffer.data[255][15])+2.0-0.5;

	int texlookup = texelFetch1D(mcubesLookup2, 0, 0).a;
	float test = float(texlookup) - 0.5;
  	gl_FragColor = vec4(test,test,test,1.0);
}
