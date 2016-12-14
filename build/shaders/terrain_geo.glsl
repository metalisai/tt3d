#version 440
#extension ARB_enhanced_layouts:enable

layout(points) in;
layout(triangle_strip, max_vertices = 128) out;

uniform isampler2D mcubesLookup;
uniform isampler1D mcubesLookup2;

uniform mat4 transformMatrix;
uniform mat4 perspectiveMatrix;
uniform vec3 worldOffset;
uniform float voxelScale;

out vec4 outVertPos;

layout(location = 1) smooth out vec3 theNormal;
layout(location = 3) smooth out vec2 Texcoord;

vec3 a2fVertexOffsetv[8] =
{
    vec3(0.0, 0.0, 0.0),vec3(1.0, 0.0, 0.0),vec3(1.0, 1.0, 0.0),vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),vec3(1.0, 0.0, 1.0),vec3(1.0, 1.0, 1.0),vec3(0.0, 1.0, 1.0)
};

int a2iEdgeConnection[12][2] =
{
    {0,1}, {1,2}, {2,3}, {3,0},
    {4,5}, {5,6}, {6,7}, {7,4},
    {0,4}, {1,5}, {2,6}, {3,7}
};

vec3 a2fEdgeDirectionv[12] =
{
    vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(-1.0, 0.0, 0.0),vec3(0.0, -1.0, 0.0),
    vec3(1.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0),vec3(-1.0, 0.0, 0.0),vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, 1.0),vec3(0.0, 0.0, 1.0),vec3( 0.0, 0.0, 1.0),vec3(0.0,  0.0, 1.0)
};

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

float getOffset(float v1, float v2)
{
	float delta = v1 - v2;
   if(delta == 0.0)
		return 0.5;
	return v1/delta;
}

vec3 getClosestPointOnLine(vec3 a, vec3 b, vec3 from)
{
    vec3 atob = b-a;
	 from = from-a;

    float atob2 = dot(atob,atob); // squared length

    float atopDotAtob = dot(from, atob);
    float t = atopDotAtob / atob2;
    t = clamp(t,0.0,1.0);

    vec3 res;
	 atob *= t;

    return a+atob;
}

float voxel(vec3 worldPos)
{
	float lscale = 0.05;

	vec3 hsl = worldPos;
	hsl.y = 1.11;
	float maxHeight = 1.0-step((snoise(lscale*hsl)+1)*2.5+2.0, worldPos.y);
	hsl.y = 33.11;

	float h1 = (snoise(lscale*hsl)+1)*2.5;
	lscale = 0.005;
	float h2 = (snoise(lscale*hsl)+1)*20.5;

	float minHeight = (h1+h2) - worldPos.y ;

	lscale = 0.05;

	float discardRules = maxHeight;
	float addRules = minHeight;

	vec3 p0 = vec3(5.0,0.0,5.0);
	vec3 p1 = vec3(50.0,20.0,200.0);
	vec3 cp = getClosestPointOnLine(p0,p1,worldPos);

	float sphere = length(worldPos-cp);
		if(sphere < 5.0)
			return (-5.0+sphere)*10.0; // I don't know why 10 makes it smoother

	p0 = vec3(20.0,0.0,5.0);
	p1 = vec3(70.0,50.0,200.0);
	cp = getClosestPointOnLine(p0,p1,worldPos);

	sphere = length(worldPos-cp);
	if(sphere < 5.0)
		return (5.0-sphere)*10.0;

	return /*discardRules * snoise(lscale*worldPos) + */addRules;
}

void vMarchCube1(vec3 wcoord)
{
    int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
    float fOffset;
    vec3 asEdgeVertex[12];

	 float afCubeValuem[17][17][17];
    for(int i = 0; i < 17; i++){
    	for(int j = 0; j < 17; j++){
    		for(int k = 0; k < 17; k++){
    			afCubeValuem[i][j][k] = voxel((wcoord + voxelScale*vec3(float(i),float(j),float(k))));
 			}
 		}
    }

    /*iFlagIndex = 0;

    vec4 afCubeValuev[2];
    afCubeValuev[0].x = voxel((wcoord + voxelScale*a2fVertexOffsetv[0]));
    afCubeValuev[0].y = voxel((wcoord + voxelScale*a2fVertexOffsetv[1]));
    afCubeValuev[0].z = voxel((wcoord + voxelScale*a2fVertexOffsetv[2]));
    afCubeValuev[0].w = voxel((wcoord + voxelScale*a2fVertexOffsetv[3]));
    afCubeValuev[1].x = voxel((wcoord + voxelScale*a2fVertexOffsetv[4]));
    afCubeValuev[1].y = voxel((wcoord + voxelScale*a2fVertexOffsetv[5]));
    afCubeValuev[1].z = voxel((wcoord + voxelScale*a2fVertexOffsetv[6]));
    afCubeValuev[1].w = voxel((wcoord + voxelScale*a2fVertexOffsetv[7]));

	 // I heard GPU likes vectors...
    iFlagIndex = int(dot(ivec4(1, 2, 4, 8)*ivec4(step(afCubeValuev[0],vec4(0.0))),ivec4(1)) + 
		dot(ivec4(16, 32, 64, 128)*ivec4(step(afCubeValuev[1],vec4(0.0))),ivec4(1)) + vec4(0.1)); */

	 float afCubeValue[8];
	 afCubeValue[0] = afCubeValuem[0][0][0];
    afCubeValue[1] = afCubeValuem[1][0][0];
    afCubeValue[2] = afCubeValuem[1][1][0];
    afCubeValue[3] = afCubeValuem[0][1][0];
    afCubeValue[4] = afCubeValuem[0][0][1];
    afCubeValue[5] = afCubeValuem[1][0][1];
    afCubeValue[6] = afCubeValuem[1][1][1];
    afCubeValue[7] = afCubeValuem[0][1][1];
	 iFlagIndex = 0;
    for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
    {
        if(afCubeValue[iVertexTest] <= 0.0f)
                iFlagIndex |= 1<<iVertexTest;
    }

    iEdgeFlags = texelFetch1D(mcubesLookup2, iFlagIndex, 0).a;
    if(iEdgeFlags == 0)
    {
        return;
    }

    for(iEdge = 0; iEdge < 12; iEdge++)
    {
    	  if(iEdgeFlags & (1<<iEdge))
        {
    		   fOffset = getOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],
                                         afCubeValue[ a2iEdgeConnection[iEdge][1]]);
  	   	   asEdgeVertex[iEdge] = wcoord + voxelScale*(a2fVertexOffsetv[a2iEdgeConnection[iEdge][0]] + fOffset * a2fEdgeDirectionv[iEdge]);
		  }
    }

	 vec4 opos[3];
	 vec4 umverts[3];
    for(iTriangle = 0; iTriangle < 5; iTriangle++)
    {
        if(texelFetch2D(mcubesLookup, ivec2(3*iTriangle, iFlagIndex), 0).a > -1)
        {
				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iTriangle+0, iFlagIndex), 0).a;
				umverts[0] = vec4(asEdgeVertex[iVertex], 1.0);
				opos[0] = perspectiveMatrix*transformMatrix*umverts[0];

				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iTriangle+1, iFlagIndex), 0).a;
				umverts[1] = vec4(asEdgeVertex[iVertex], 1.0);
				opos[1] = perspectiveMatrix*transformMatrix*umverts[1];

				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iTriangle+2, iFlagIndex), 0).a;
				umverts[2] = vec4(asEdgeVertex[iVertex], 1.0);
				opos[2] = perspectiveMatrix*transformMatrix*umverts[2];

				vec3 normal = normalize(cross(umverts[1].xyz - umverts[0].xyz, umverts[2].xyz - umverts[0].xyz));

				outVertPos = umverts[0];
				gl_Position = opos[0]; // have to emit here because we need all 3 vertices to calc normal
				theNormal = normal;
				EmitVertex();

				outVertPos = umverts[1];
				gl_Position = opos[1];
				theNormal = normal;
				EmitVertex();

				outVertPos = umverts[2];
				gl_Position = opos[2];
				theNormal = normal;
				EmitVertex();

				EndPrimitive();
        }
    }
}

void main()
{   
	for(int j = -1; j < 100; j++)
	{
		vMarchCube1(vec3(gl_in[0].gl_Position.x,float(j)*voxelScale,gl_in[0].gl_Position.z) + worldOffset);
	}
}
