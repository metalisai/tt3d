iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+0, iFlagIndex), 0).a;
outputVertexBuffer.data[triangleIndex].position0 = asEdgeVertex[iVertex];

iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+1, iFlagIndex), 0).a;
outputVertexBuffer.data[triangleIndex].position1 = asEdgeVertex[iVertex];

iVertex = texelFetch2D(mcubesLookup, ivec2(3*iterate+2, iFlagIndex), 0).a;
outputVertexBuffer.data[triangleIndex].position2 = asEdgeVertex[iVertex];





asEdgeVertex[0] = edgeVertexData.data[index].left[i][j][k][0];
asEdgeVertex[1] = edgeVertexData.data[index].left[j][i+1][k][1];
asEdgeVertex[2] = edgeVertexData.data[index].left[i][j+1][k][0];
asEdgeVertex[3] = edgeVertexData.data[index].left[j][i][k][1];

asEdgeVertex[4] = edgeVertexData.data[index].left[i][j][k+1][0];
asEdgeVertex[5] = edgeVertexData.data[index].left[j][i+1][k+1][1];
asEdgeVertex[6] = edgeVertexData.data[index].left[i][j+1][k+1][0];
asEdgeVertex[7] = edgeVertexData.data[index].left[j][i][k+1][1];

asEdgeVertex[8] = edgeVertexData.data[index].left[k][i][j][2];
asEdgeVertex[9] = edgeVertexData.data[index].left[k][i+1][j][2];
asEdgeVertex[10] = edgeVertexData.data[index].left[k][i+1][j+1][2];
asEdgeVertex[11] = edgeVertexData.data[index].left[k][i][j+1][2];
