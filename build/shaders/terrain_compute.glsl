#version 440

/*struct Vertex {
	vec4 position;
};*/

struct VertexOut {
	vec4 position0;
	vec4 normal0;
	vec4 position1;
	vec4 normal1;
	vec4 position2;
	vec4 normal2;
};

struct Line {
    vec4 start;
    vec4 end;
};

struct GenData
{
	uint tunnelCount;
   float firstOctaveMax;
   float secondOctaveMax;
   float padding;
   float dxgoalFirstOctaveMax;
   float dxgoalSecondOctaveMax;
	float dzgoalFirstOctaveMax;
   float dzgoalSecondOctaveMax;
	vec4 chunkOrigin;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer InputVertexBuffer {
	vec4 data[];
} inputVertexBuffer;

layout(std430, binding = 1) writeonly buffer OutputVertexBuffer {
	VertexOut data[];
} outputVertexBuffer;

layout(std430, binding = 2) readonly buffer TunnelData
{
    GenData genData;
    Line tunnels[];
} tunnelData;

// Uniforms
uniform isampler2D mcubesLookup;
uniform isampler1D mcubesLookup2;

uniform vec3 worldOffset;
uniform float voxelScale;

layout (binding = 3, offset = 0) uniform atomic_uint triangleCount;

// Shared values between all the threads in the group

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
	 atob *= t;
    return a+atob;
}

float voxel(vec3 worldPos)
{
   vec3 curVec = worldPos-tunnelData.genData.chunkOrigin.xyz;
   float progressX = curVec.x/64.0; 	// TODO: 100=chunk size
   float progressZ = curVec.z/64.0;
	float fom = mix(0.0,tunnelData.genData.dxgoalFirstOctaveMax,progressX) + mix(0.0,tunnelData.genData.dzgoalFirstOctaveMax,progressZ);
   float som = mix(0.0,tunnelData.genData.dxgoalSecondOctaveMax,progressX) + mix(0.0,tunnelData.genData.dzgoalSecondOctaveMax,progressZ);

	float warp = (snoise(0.08*worldPos)+1)*0.5;
	vec3 sampleCoord = vec3(0.5,0.9,0.68)*warp*10 + worldPos;
	sampleCoord.y = 33.11;

	float h1 = (snoise(0.05*sampleCoord)+1)*(tunnelData.genData.firstOctaveMax+fom);
	float h2 = (snoise(0.005*sampleCoord)+1)*(tunnelData.genData.secondOctaveMax+som);

	float minHeight = (h1+h2) - worldPos.y ;

	for(int i = 0; i < tunnelData.genData.tunnelCount; i++)
	{
		vec3 p0 = tunnelData.tunnels[i].start.xyz;
		vec3 p1 = tunnelData.tunnels[i].end.xyz;
		vec3 cp = getClosestPointOnLine(p0,p1,worldPos);
		float sphere = length(worldPos-cp);
		if(sphere < 5.0)
			return (-5.0+sphere)*10.0;
	}

	return minHeight;
}

void vMarchCube1(vec3 wcoord)
{	

    int iVertex, iterate, iFlagIndex, iEdgeFlags;
    float fOffset;
    vec3 asEdgeVertex[12];

	 float afCubeValue[8];
	 afCubeValue[0] = voxel((wcoord + voxelScale*a2fVertexOffsetv[0]));
    afCubeValue[1] = voxel((wcoord + voxelScale*a2fVertexOffsetv[1]));
    afCubeValue[2] = voxel((wcoord + voxelScale*a2fVertexOffsetv[2]));
    afCubeValue[3] = voxel((wcoord + voxelScale*a2fVertexOffsetv[3]));
    afCubeValue[4] = voxel((wcoord + voxelScale*a2fVertexOffsetv[4]));
    afCubeValue[5] = voxel((wcoord + voxelScale*a2fVertexOffsetv[5]));
    afCubeValue[6] = voxel((wcoord + voxelScale*a2fVertexOffsetv[6]));
    afCubeValue[7] = voxel((wcoord + voxelScale*a2fVertexOffsetv[7]));
	 iFlagIndex = 0;
    for(iterate = 0; iterate < 8; iterate++)
    {
        if(afCubeValue[iterate] <= 0.0f)
                iFlagIndex |= 1<<iterate;
    }

    iEdgeFlags = texelFetch1D(mcubesLookup2, iFlagIndex, 0).a;
    if(iEdgeFlags == 0)
    {
        return;
    }

    for(iterate = 0; iterate < 12; iterate++)
    {
    	  if(iEdgeFlags & (1<<iterate))
        {
    		   fOffset = getOffset(afCubeValue[ a2iEdgeConnection[iterate][0] ],
                                         afCubeValue[ a2iEdgeConnection[iterate][1]]);
  	   	   asEdgeVertex[iterate] = wcoord + voxelScale*(a2fVertexOffsetv[a2iEdgeConnection[iterate][0]] + fOffset * a2fEdgeDirectionv[iterate]);
		  }
    }

	 vec4 umverts[3];
    for(iterate = 0; iterate < 5; iterate++)
    {
        if(texelFetch2D(mcubesLookup, ivec2(3*iterate, iFlagIndex), 0).a > -1)
        {
        		uint triangleIndex = atomicCounterIncrement(triangleCount); 

				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+0, iFlagIndex), 0).a;
				if(asEdgeVertex[iVertex].x == 0.0)
					asEdgeVertex[iVertex].x += 0.00000001;
				outputVertexBuffer.data[triangleIndex].position0 = vec4(asEdgeVertex[iVertex].x,asEdgeVertex[iVertex].y,asEdgeVertex[iVertex].z, asEdgeVertex[iVertex].x/asEdgeVertex[iVertex].x);

				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+1, iFlagIndex), 0).a;
				if(asEdgeVertex[iVertex].x == 0.0)
					asEdgeVertex[iVertex].x += 0.00000001;
				outputVertexBuffer.data[triangleIndex].position1 = vec4(asEdgeVertex[iVertex].x,asEdgeVertex[iVertex].y,asEdgeVertex[iVertex].z, asEdgeVertex[iVertex].x/asEdgeVertex[iVertex].x); // WHY THE FUCK DOESNT IT WRITE A CONSTANT???????

				iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+2, iFlagIndex), 0).a;
				if(asEdgeVertex[iVertex].x == 0.0)
					asEdgeVertex[iVertex].x += 0.00000001;
				outputVertexBuffer.data[triangleIndex].position2 = vec4(asEdgeVertex[iVertex].x,asEdgeVertex[iVertex].y,asEdgeVertex[iVertex].z, asEdgeVertex[iVertex].x/asEdgeVertex[iVertex].x);

				vec3 normal = normalize(cross(outputVertexBuffer.data[triangleIndex].position1.xyz - outputVertexBuffer.data[triangleIndex].position0.xyz, outputVertexBuffer.data[triangleIndex].position2.xyz - outputVertexBuffer.data[triangleIndex].position0.xyz));

				outputVertexBuffer.data[triangleIndex].normal0 = vec4(normal,0.0);
				outputVertexBuffer.data[triangleIndex].normal1 = vec4(normal,0.0);
				outputVertexBuffer.data[triangleIndex].normal2 = vec4(normal,0.0);
        }
    }
}

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() {
	ivec2 location = ivec2(gl_WorkGroupID.xy);
	ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
	//ivec2 tileID = ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
	uint index = location.x;

	barrier();

	for(int j = 0; j < 60; j++)
	{
		vMarchCube1(vec3(inputVertexBuffer.data[index].x + (itemID.x*voxelScale),float(j)*voxelScale,inputVertexBuffer.data[index].z + (itemID.y*voxelScale)) + worldOffset);
	}
}
