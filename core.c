#include "core.h"

#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

Mesh *domeMesh, *mesh, *terrainMesh, *vterrainMesh[100];

int a2iTriangleConnectionTable2[256][16] =
{
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

GLint aiCubeEdgeFlags2[256]=
{
    0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, 0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, 0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c, 0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac, 0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c, 0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc, 0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c, 0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc, 0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, 0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, 0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, 0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, 0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, 0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, 0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, 0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, 0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};

//Mesh *terrainMesh[10][10]/*= generateJunk(0,0,0)*/;

#define SHADOWMAP_RES   2048

void addEntity(Permanent_Storage *state, Entity *ent)
{
    state->entities[state->numEntities] = ent;
    ++state->numEntities;
}

static void addSurfaceShader(Permanent_Storage *state, Shader *shader)
{
    state->shaders[state->numShaders] = shader;
    ++state->numShaders;
}

#define NUM_LIGHTS 1024
void initLights(Permanent_Storage *state)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->fplus.lightBuffer);
    PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

    for (int i = 0; i < NUM_LIGHTS/16; i++) {
        PointLight *light = &pointLights[i];
        light->position = vec4((random()%10000)/100.f,0.5f,(random()%10000)/100.f, 1.0f);
        light->color = vec4(1.0f -(random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 1.0f - (random()%1000)/1000.f, 0.5f);
        light->paddingAndRadius = vec4FromVec3AndW(vec3(0.0f,0.0f,0.0f), 15.f); // TODO: add light radius
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void initMCubesBuffer(Permanent_Storage *state)
{
    int ok;
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &ok);
    printf("well then %d \n",ok);
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &ok);
    printf("well then2 %d \n",ok);
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &ok);
    printf("well then3 %d \n",ok);
    // Generate our shader storage buffers
    glGenBuffers(1, &state->mcubesBuffer);
    // Bind light buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->mcubesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(a2iTriangleConnectionTable2), a2iTriangleConnectionTable2, GL_STATIC_DRAW);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->mcubesBuffer);
    i32 *buffer = (i32*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
    memcpy(buffer, a2iTriangleConnectionTable2, sizeof(a2iTriangleConnectionTable2));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, state->mcubesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void initMCubesBuffer2(Permanent_Storage *state)
{
    glGenTextures(1, &state->mcubesTexture);
    glBindTexture(GL_TEXTURE_2D, state->mcubesTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 16, 256, 0, GL_ALPHA_INTEGER_EXT, GL_INT, a2iTriangleConnectionTable2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &state->mcubesTexture2);
    glBindTexture(GL_TEXTURE_1D, state->mcubesTexture2);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_ALPHA16I_EXT, 256, 0, GL_ALPHA_INTEGER_EXT, GL_INT, aiCubeEdgeFlags2);
    int error = glGetError();
    printf("mcubestexerroe: %d\n",error);
    glBindTexture(GL_TEXTURE_1D, 0);

    int ok;
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &ok);
    printf("well then2 %d \n",ok);

    /* Transform beedback */
    printf("transform feedback before: %d and %d\n",state->tfeedbackBuffer, state->tfeedback);
    //glGenTransformFeedbacks(1, &state->tfeedback);
    glGenBuffers(1, &state->tfeedbackBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, state->tfeedbackBuffer);
    glBufferData(GL_ARRAY_BUFFER, 200*1024*1024, 0, GL_STATIC_READ);

    //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,state->tfeedback);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,state->tfeedbackBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    printf("transform feedback after: %d %d\n",state->tfeedbackBuffer, state->tfeedback);
    printf("transform feedback error: %d\n",glGetError());

    /*
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &ok);
    printf("well then %d \n",ok);
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &ok);
    printf("well then2 %d \n",ok);
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &ok);
    printf("well then3 %d \n",ok);
    // Generate our shader storage buffers
    glGenBuffers(1, &state->mcubesBuffer);
    // Bind light buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->mcubesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(a2iTriangleConnectionTable2), a2iTriangleConnectionTable2, GL_STATIC_DRAW);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/

    /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->mcubesBuffer);
    i32 *buffer = (i32*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
    memcpy(buffer, a2iTriangleConnectionTable2, sizeof(a2iTriangleConnectionTable2));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, state->mcubesBuffer);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void updateLights(Permanent_Storage *state, r32 intensity)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->fplus.lightBuffer);
    int lights = 1;
    PointLight *pointLights = (PointLight*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,lights*sizeof(PointLight),GL_MAP_WRITE_BIT);
    for (int i = 0; i < lights; i++) {
        PointLight *light = &pointLights[i];
        light->color.w = intensity;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void createParticleSystem(ParticleSystem *particles)
{
    //int vertices = particles->maxParticles;
    int vertices = particles->maxParticles;

    u32 bufferSize = vertices*(sizeof(Vec3)+sizeof(r32));
    //void* data = malloc(bufferSize);
    glGenBuffers(1, &particles->GLBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles->GLBuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, particles->particleData, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &particles->VAO);
    glBindVertexArray(particles->VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*glGenBuffers(1, &mesh->ElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    int byteOffset = vertices*sizeof(Vec4)+vertices*sizeof(Vec3)+vertices*sizeof(Vec2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->faces*3*2, ((char*)data)+byteOffset, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
    /*glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);*/
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3)+sizeof(r32), 0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vec3)+sizeof(r32), (GLvoid*)(0+sizeof(Vec3)));
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    //glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderAndUpdateParticleSystemOrig(ParticleSystem *particles, r32 dt)
{
    float particleSpawnRate = 0.5f;
    particles->timeSinceLastParticle +=dt;
    if(particles->timeSinceLastParticle >= particleSpawnRate)
    {
        particles->particles[particles->lastDestroyedIndex].localPosition =  vec3(0.f,0.f,0.f);
        particles->particles[particles->lastDestroyedIndex].timer = 0.f;
        particles->particles[particles->lastDestroyedIndex].fade = 1.f;
        particles->lastSpawnedIndex = particles->lastDestroyedIndex;
        particles->lastDestroyedIndex = (particles->lastDestroyedIndex+1)%particles->maxParticles;
        //printf("Particle spawned\n");
    }

    for(int i = 0; i < particles->maxParticles; i++)
    {
        particles->particles[i].timer += dt;
        particles->particles[i].fade = 1.f-particles->particles[i].timer/particles->lifeTime;
        Vec3 scaledVel;
        vec3Scale(&scaledVel, &particles->velocity, dt);
        vec3Add(&particles->particles[i].localPosition, &particles->particles[i].localPosition, &scaledVel);
        vec3Add(&particles->particleData[i].position, &particles->position, &particles->particles[i].localPosition);
        //particles->particleData[i].position = particles->position+particles->particles[i].localPosition;
        particles->particleData[i].fade = 1.f;
    }

    //printf("P0a pos %f %f %f \n",particles->particleData[0].position.x,particles->particleData[0].position.y,particles->particleData[0].position.z);

    int vertices = particles->maxParticles;
    u32 bufferSize = vertices*(sizeof(Vec3)+sizeof(r32));

    glBindBuffer(GL_ARRAY_BUFFER, particles->GLBuffer);
    glBindVertexArray(particles->VAO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, &particles->particleData, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// terrain
void renderAndUpdateParticleSystem(ParticleSystem *particles, r32 dt)
{
    float particleSpawnRate = 0.5f;
    particles->timeSinceLastParticle +=dt;
    if(particles->timeSinceLastParticle >= particleSpawnRate)
    {
        particles->particles[particles->lastDestroyedIndex].localPosition =  vec3(0.f,0.f,0.f);
        particles->particles[particles->lastDestroyedIndex].timer = 0.f;
        particles->particles[particles->lastDestroyedIndex].fade = 1.f;
        particles->lastSpawnedIndex = particles->lastDestroyedIndex;
        particles->lastDestroyedIndex = (particles->lastDestroyedIndex+1)%particles->maxParticles;
        //printf("Particle spawned\n");
    }

    particles->particles[particles->maxParticles].timer += dt;

    int index = 0;
    for(int i = 0; i < 100; i++)
    {
        for(int j = 0; j < 100; j++)
        {
            //particles->particles[i].timer += dt;
            //particles->particles[i].fade = 1.f-particles->particles[i].timer/particles->lifeTime;
            //Vec3 scaledVel;
            //vec3Scale(&scaledVel, &particles->velocity, dt);
            //vec3Add(&particles->particles[i].localPosition, &particles->particles[i].localPosition, &scaledVel);
            //vec3Add(&particles->particleData[i].position, &particles->position, &particles->particles[i].localPosition);
            particles->particleData[index].position = vec3(i*1.0f, particles->particles[particles->maxParticles].timer, j*1.0f);
            //particles->particleData[i].position = particles->position+particles->particles[i].localPosition;
            //particles->particleData[i].fade = 1.f;
            index++;
        }
    }

    //printf("P0a pos %f %f %f \n",particles->particleData[0].position.x,particles->particleData[0].position.y,particles->particleData[0].position.z);

    int vertices = particles->maxParticles;
    u32 bufferSize = vertices*(sizeof(Vec3)+sizeof(r32));

    glBindBuffer(GL_ARRAY_BUFFER, particles->GLBuffer);
    glBindVertexArray(particles->VAO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, &particles->particleData, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_POINTS, 0, 100*100);
    //int err = glGetError();
    //printf("ge:%d\n",err);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PlatformApi Platform;
void init(EngineMemory *mem, int width, int height)
{
    Platform = mem->platformApi;
    memset(mem->gameState, 0, 100LL*1024LL*1024LL);
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;
    state->deferred = true;

    cameraInitialize(&(state->main_cam));
    state->main_cam.FOV = 45.f;
    state->main_cam.nearPlane = 0.1f;
    state->main_cam.farPlane = 200.f;
    state->numEntities = 0;
    state->numShaders = 0;
    state->captured = 0;

    openglCreateDeferredFBO(&state->defferedFbo, width, height);
    GLfloat fbo_vertices[] = {
        -1, -1,
        1, -1,
        -1,  1,
        1,  1,
    };
    // TODO: cleanup this?
    glGenBuffers(1, &state->vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_fbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    openglCreateDepthFBO(&state->shadowmap_fbo, SHADOWMAP_RES, SHADOWMAP_RES, true);
    //genShadowBuffer(state, SHADOWMAP_RES, SHADOWMAP_RES);

    // forward plus stuff
    openglCreateDepthFBO(&state->zprepass_fbo, width, height, false);
    openglCreateLightBuffers(&state->fplus.workGroupsX,&state->fplus.workGroupsY, &state->fplus.lightBuffer, &state->fplus.visibleLightIndicesBuffer, width, height);
    initLights(state);
    printf("before: %d\n",state->mcubesBuffer);
    initMCubesBuffer(state);
    printf("after: %d\n",state->mcubesBuffer);

    printf("before2: %d\n",state->mcubesTexture);
    initMCubesBuffer2(state);
    printf("after2: %d\n",state->mcubesTexture);


    stackInit((MemStack*)mem->transientState, ((char*)mem->transientState)+sizeof(MemStack), 30LL*1024LL*1024LL);
    state->texturePool = createTexturePool(1024, (MemStack*)mem->transientState);
    state->deferred = true;

    setupDebug();

    InitText();

    domeMesh = loadMesh("sphere.tt");
    mesh = loadMesh("barra/barra.tt");
    terrainMesh = generateTerrainMesh();

    LoadedTexture *groundTexture = loadTextureToMemory(state->texturePool, "grass.png");
    openGL_LoadTexture(groundTexture);
    LoadedTexture *barrelTexture = loadTextureToMemory(state->texturePool, "barra/barra.jpg");
    openGL_LoadTexture(barrelTexture);
    LoadedTexture *stoneTexture = loadTextureToMemory(state->texturePool, "textures/stone.jpg");
    openGL_LoadTexture(stoneTexture);
    LoadedTexture *brickDiffuseTexture = loadTextureToMemory(state->texturePool, "textures/brick_diff.jpg");
    openGL_LoadTexture(brickDiffuseTexture);
    LoadedTexture *brickNormalTexture = loadTextureToMemory(state->texturePool, "textures/brick_norm.jpg");
    openGL_LoadTexture(brickNormalTexture);

    Mesh *planeMesh = generatePlane();

    initializeProgram(&state->textShader,"shaders/textvert.glsl", "shaders/textfrag.glsl", ST_Surface);
    initializeProgram(&state->postProcShader,"shaders/postproc_vert.glsl", "shaders/postproc_frag.glsl", ST_PostProc);
    initializeProgram(&state->postProcPointShader,"shaders/postproc_vert.glsl", "shaders/postproc_point_frag.glsl", ST_PostProc);
    initializeProgram(&state->shadowPassShader,"shaders/vert_shadow_pass.glsl", "shaders/frag_shadow_pass.glsl", ST_Surface);
    initializeComputeProgram(&state->lightCullShader, "shaders/forwardp_lightcull.glsl", ST_LightCull);

    initializeProgram(&state->game.notexShader,"shaders/vert.glsl", "shaders/notex_frag.glsl", ST_Surface);
    initializeProgram(&state->game.texShader,"shaders/vert.glsl", "shaders/frag.glsl", ST_Surface);
    initializeProgram(&state->game.skyShader,"shaders/skydomevert.glsl", "shaders/skydomefrag.glsl", ST_Skydome);
    initializeProgram(&state->game.normalShader,"shaders/vert_normal.glsl", "shaders/frag_normal.glsl", ST_Surface);
    initializeProgram(&state->game.lineShader,"shaders/linevert.glsl", "shaders/linefrag.glsl", ST_Surface);
    initializeProgram(&state->game.particleShader,"shaders/particle_vert.glsl", "shaders/frag_color_forward.glsl", ST_Particle);

    initializeProgram(&state->game.texShaderForw,"shaders/vert_forward.glsl", "shaders/frag_forward.glsl", ST_Surface);
    initializeProgram(&state->game.straightShader,"shaders/straight_vert.glsl", "shaders/frag_color_forward.glsl", ST_Surface);


    addSurfaceShader(state, &state->game.notexShader);
    addSurfaceShader(state, &state->game.texShader);
    addSurfaceShader(state, &state->game.normalShader);
    addSurfaceShader(state, &state->game.texShaderForw);

    meshRecalculateNormals(mesh);
    meshRecalculateNormals(terrainMesh);
    meshRecalculateNormals(planeMesh);
    meshRecalculateNormals(domeMesh);
    openGL_loadMesh(mesh);
    openGL_loadMesh(terrainMesh);
    openGL_loadMesh(planeMesh);
    openGL_loadMesh(domeMesh);

    int k = 0;
    for(int i= 0; i < 20; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            state->game.barrel[k].mesh = *mesh;
            transformInit(&state->game.barrel[k].transform);
            state->game.barrel[k].transform.position =  vec3((float)i+10.0f,0.f,(float)j+10.0f);
            state->game.barrel[k].material.numTextures = 1;
            state->game.barrel[k].material.shader = &state->game.texShader;
            state->game.barrel[k].material.texture_handle[0] = barrelTexture->gl_handle;
            addEntity(state, &state->game.barrel[k]);
            k++;
        }
    }

    for(int i = 0; i < 4; i++)
    {
        vterrainMesh[i] = terrainGen(4.f*i);
        meshRecalculateNormals(vterrainMesh[i]);
        openGL_loadMesh(vterrainMesh[i]);

        state->game.voxelTerrain[i].mesh = *vterrainMesh[i];
        transformInit(&state->game.voxelTerrain[i].transform);
        state->game.voxelTerrain[i].transform.position =  vec3(0.0f,0.0f,0.0f);
        //state->game.voxelTerrain[i].transform.scale =  vec3(1.0f,1.0f,1.0f);
        //state->game.voxelTerrain[i].transform.rotation = quaternion(1.0f,0.0f,0.0f,0.0f);
        state->game.voxelTerrain[i].material.numTextures = 1;
        state->game.voxelTerrain[i].material.shader = &state->game.texShader;
        state->game.voxelTerrain[i].material.texture_handle[0] = state->shadowmap_fbo.depthFBO.depthTexture;
        //addEntity(state, &state->game.voxelTerrain[i]);
    }

    state->game.testBarrel.mesh = *mesh/*planeMesh*/;
    transformInit(&state->game.testBarrel.transform);
    state->game.testBarrel.material.numTextures = 1;
    state->game.testBarrel.material.shader = &state->game.texShader;
    state->game.testBarrel.material.texture_handle[0] = barrelTexture->gl_handle;
    addEntity(state, &state->game.testBarrel);

    state->game.terrain.mesh = *terrainMesh;
    transformInit(&state->game.terrain.transform);
    printf("scalex %f",state->game.terrain.transform.scale.x);
    state->game.terrain.transform.position =  vec3(0.f,0.f,0.f);
    state->game.terrain.material.numTextures = 1;
    state->game.terrain.material.shader = &state->game.texShader;
    state->game.terrain.material.texture_handle[0] = groundTexture->gl_handle;
    addEntity(state, &state->game.terrain);

    state->game.wall.mesh = *planeMesh;
    transformInit(&state->game.wall.transform);
    state->game.wall.transform.position =  vec3(10.0f,2.0f,10.0f);
    state->game.wall.material.numTextures = 2;
    state->game.wall.material.shader = &state->game.normalShader;
    state->game.wall.material.texture_handle[0] = brickDiffuseTexture->gl_handle;
    state->game.wall.material.texture_handle[1] = brickNormalTexture->gl_handle;
    addEntity(state, &state->game.wall);

    state->game.wall2.mesh = *planeMesh;
    transformInit(&state->game.wall2.transform);
    state->game.wall2.transform.position =  vec3(9.0f,2.0f,10.0f);
    state->game.wall2.material.numTextures = 2;
    state->game.wall2.material.shader = &state->game.texShader;
    state->game.wall2.material.texture_handle[0] = brickDiffuseTexture->gl_handle;
    addEntity(state, &state->game.wall2);

    /*state->game.wall3.mesh = *vterrainMesh;
    transformInit(&state->game.wall3.transform);
    state->game.wall3.transform.position =  vec3(0.0f,0.0f,0.0f);
    state->game.wall3.transform.scale =  vec3(1.0f,1.0f,1.0f);
    state->game.wall3.transform.rotation = quaternionFromAxisAngle(vec3(0.0f,1.0f,0.0f),3.14f);
    state->game.wall3.material.numTextures = 2;
    state->game.wall3.material.shader = &state->game.texShader;
    state->game.wall3.material.texture_handle[0] = state->shadowmap_fbo.depthFBO.depthTexture;
    addEntity(state, &state->game.wall3);*/


    state->game.dome.mesh = *domeMesh;
    transformInit(&state->game.dome.transform);
    state->game.dome.transform.position =  vec3(0.0f,0.0f,0.0f);;
    state->game.dome.material.numTextures = 0;
    state->game.dome.material.shader = &state->game.skyShader;
    //dome.transform.scale = Vec3(3500.0f,3500.0f,3500.0f);
    state->game.dome.transform.scale =  vec3(200.0f,200.0f,200.0f);
    //addEntity(&dome);

    state->game.earth.mesh = *domeMesh;
    transformInit(&state->game.earth.transform);
    state->game.earth.transform.position =  vec3(0.0f,0.0f,0.0f);;
    state->game.earth.material.numTextures = 0;
    state->game.earth.material.shader = &state->game.texShader;
    state->game.earth.material.numTextures = 1;
    state->game.earth.material.texture_handle[0] = stoneTexture->gl_handle;
    //earth.transform.scale =  vec3(3000.0f,3000.0f,3000.0f);
    float planetSize = 15000.f;
    state->game.earth.transform.scale =  vec3(planetSize,planetSize,planetSize);
    //addEntity(&earth);

    state->game.particles.lifeTime = 10.f;
    state->game.particles.maxParticles = 100000;
    state->game.particles.lastSpawnedIndex = 0;
    state->game.particles.position =  vec3(10.f,0.f,10.f);
    state->game.particles.texture = *barrelTexture;
    state->game.particles.velocity =  vec3(0.2f,1.0f,0.0f);
    state->game.particles.lastDestroyedIndex = 0;
    state->game.particles.lastSpawnedIndex = 0;
    state->game.particles.timeSinceLastParticle = 0.0f;
    createParticleSystem(&state->game.particles);



    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    if(!glCreateShader(GL_GEOMETRY_SHADER)){
        printf("Creating geos failed\n");
    }

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int frames = 0;

void visibilityCheck(Permanent_Storage *state)
{
    Plane frustumPlanes[6];
    getFrustumPlanes(&state->main_cam.perspectiveMatrix,frustumPlanes);
    for(int i = 0; i < 6; i++)
    {
        Vec4 thing = vec4(frustumPlanes[i].a,frustumPlanes[i].b,frustumPlanes[i].c,0.0f);
        Mat4 mat = cameraCalculateInverseViewMatrix(&state->main_cam);
        Vec4 res;
        mat4Vec4Mul(&res, &mat, &thing);
        frustumPlanes[i] = planeFromVec4(&res);
    }

    Vec3 n[6];
    r32 d[6];

    for(int planeId = 0; planeId < 6; planeId++)
    {
        Vec3 idk = vec3(frustumPlanes[planeId].a,frustumPlanes[planeId].b,frustumPlanes[planeId].c);
        n[planeId] =  vec3Normalized(&idk);
        d[planeId] = frustumPlanes[planeId].d;
    }

    // TODO: float.max
    Vec3* minV = &state->minimumPositions;
    Vec3* maxV = &state->maximumPositions;

    *minV = vec3(9999999.f,999999.f,999999.f);
    *maxV = vec3(-9999999.f,-999999.f,-999999.f);

    for(int i = 0; i < state->numEntities; i++)
    {
        // TODO: check for all 6 planes?
        Vec3 pos;
        vec3Sub(&pos, &state->entities[i]->transform.position, &state->main_cam.position);
        r32 res = vec3Dot(&pos, &n[0]) + d[0]+state->entities[i]->mesh.boundingRadius*state->entities[i]->transform.scale.x;
        r32 res2 = vec3Dot(&pos, &n[1]) + d[1]+state->entities[i]->mesh.boundingRadius*state->entities[i]->transform.scale.x;
        if(res <= 0.f || res2 <= 0.f)
            state->entities[i]->visible = false;
        else
        {
            *minV = vec3(min(state->entities[i]->transform.position.x, minV->x),
                        min(state->entities[i]->transform.position.y, minV->y),
                        min(state->entities[i]->transform.position.z, minV->z));
            *maxV = vec3(max(state->entities[i]->transform.position.x, maxV->x),
                        max(state->entities[i]->transform.position.y, maxV->y),
                        max(state->entities[i]->transform.position.z, maxV->z));
            state->entities[i]->visible = true;
        }
    }
}

void deferredRender(Permanent_Storage *state, Input *input, float dt)
{
    Mat4 modelMatrix;
    Mat4 posToShadowSpace;


    // ------------------------- SHADOW PASS ------------------------------------------

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->shadowmap_fbo.fboHandle);
    glViewport(0, 0, (GLsizei) SHADOWMAP_RES, (GLsizei) SHADOWMAP_RES);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    Quaternion q = quaternion(-state->game.sunRot2Quat.w,state->game.sunRot2Quat.x,state->game.sunRot2Quat.y,state->game.sunRot2Quat.z);
    Mat4 lightTransformMatrix;
    mat4FromQuaternion(&lightTransformMatrix, &q);

    Mat4 specialPerspective = projMatrix(state->main_cam.FOV, state->main_cam.aspectRatio, state->main_cam.nearPlane, 20.f);
    Mat4 mat = cameraCalculateInverseViewMatrix(&state->main_cam);
    Mat4 mat2 = invPerspective(&specialPerspective);
    Mat4 camInvPerspective;
    mat4Mul(&camInvPerspective, &mat, &mat2);

    Vec3 frust[8];
    //left
        // down
    frust[0]=  vec3(-1.f,-1.f,-1.f);
    frust[1]=  vec3(-1.f,-1.f,1.f);
        // up
    frust[2]= vec3(-1.f,1.f,-1.f);
    frust[3]= vec3(-1.f,1.f,1.f);
    //right
    frust[4]= vec3(1.f,-1.f,-1.f);
    frust[5]= vec3(1.f,-1.f,1.f);
    frust[6]= vec3(1.f,1.f,-1.f);
    frust[7]= vec3(1.f,1.f,1.f);

    /*r32 minX = state->m;
    r32 minY = 99999999.f;
    r32 minZ = 99999999.f;
    r32 maxX = -99999999.f;
    r32 maxY = -99999999.f;
    r32 maxZ = -99999999.f;*/

    /*for(int i = 0; i < 8; i++)
    {
        Vec4 row = vec4FromVec3AndW(frust[i],1.0f);
        Vec4 sdf;
        mat4Vec4Mul(&sdf, &camInvPerspective, &row);
        vec4Scale(&sdf, &sdf, 1.0f/sdf.w);
        frust[i] = vec3FromVec4(sdf);
        minX = min(minX, frust[i].x);
        minY = min(minY, frust[i].y);
        minZ = min(minZ, frust[i].z);
        maxX = max(maxX, frust[i].x);
        maxY = max(maxY, frust[i].y);
        maxZ = max(maxZ, frust[i].z);
    }

    //drawLine(frust[1]*0.99f,frust[2]*0.99f);

    static Vec3 thing[8];
    thing[0] = vec3(minX, minY, minZ);
    thing[1] = vec3(maxX, minY, minZ);
    thing[2] = vec3(minX, minY, maxZ);
    thing[3] = vec3(maxX, minY, maxZ);
    thing[4] = vec3(minX, maxY, minZ);
    thing[5] = vec3(maxX, maxY, minZ);
    thing[6] = vec3(minX, maxY, maxZ);
    thing[7] = vec3(maxX, maxY, maxZ);


    drawLine(thing[0],thing[1]);
    drawLine(thing[2],thing[3]);
    drawLine(thing[0],thing[2]);
    drawLine(thing[1],thing[3]);

    drawLine(thing[0],thing[4]);
    drawLine(thing[1],thing[5]);
    drawLine(thing[2],thing[6]);
    drawLine(thing[3],thing[7]);

    drawLine(thing[4],thing[5]);
    drawLine(thing[6],thing[7]);
    drawLine(thing[4],thing[6]);
    drawLine(thing[5],thing[7]);*/

    /*if(getKeyDown(input, KEYCODE_F))
    {
        thing[0] = Vec3(minX,minY,minZ);
        thing[1] = Vec3(minX,minY,maxZ);
        thing[2] = Vec3(minX,maxY,minZ);
        thing[3] = Vec3(minX,maxY,maxZ);
        thing[4] = Vec3(maxX,minY,minZ);
        thing[5] = Vec3(maxX,minY,maxZ);
        thing[6] = Vec3(maxX,maxY,minZ);
        thing[7] = Vec3(maxX,maxY,maxZ);

        setPosition(&state->game.barrel[0].transform,Vec3(minX,minY,minZ));
        setPosition(&state->game.barrel[1].transform,Vec3(minX,minY,maxZ));
        setPosition(&state->game.barrel[2].transform,Vec3(minX,maxY,minZ));
        setPosition(&state->game.barrel[3].transform,Vec3(minX,maxY,maxZ));
        setPosition(&state->game.barrel[4].transform,Vec3(maxX,minY,minZ));
        setPosition(&state->game.barrel[5].transform,Vec3(maxX,minY,maxZ));
        setPosition(&state->game.barrel[6].transform,Vec3(maxX,maxY,minZ));
        setPosition(&state->game.barrel[7].transform,Vec3(maxX,maxY,maxZ));
    }*/

    /*printf("%f %f %f %f %f %f\n",
           state->minimumPositions.x,
           state->maximumPositions.x,
           state->minimumPositions.y,
           state->maximumPositions.y,
           state->minimumPositions.z,
           state->maximumPositions.z);*/

    Mat4 orthoMat = ortho(state->minimumPositions.x,
                          state->maximumPositions.x,
                          state->minimumPositions.y-40.f,
                          state->maximumPositions.y+20.f,
                          state->minimumPositions.z,
                          state->maximumPositions.z);

    Mat4 civm = cameraCalculateInverseViewMatrix(&state->main_cam);
    mat4Mul(&posToShadowSpace, &orthoMat, &lightTransformMatrix);
    mat4Mul(&posToShadowSpace, &posToShadowSpace, &civm);

    glUseProgram(state->shadowPassShader.program);
    for(int i = 0; i < state->numEntities; i++)
    {
        //if(state->entities[i]->visible) // TODO: this does not work for shadow pass, because the camera is in different location?
        {
            modelMatrix = calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &lightTransformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->shadowPassShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&orthoMat);
            glUniformMatrix4fv(state->shadowPassShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*)&modelMatrix);
            //state->entities[i]->material.
            renderMesh(&state->entities[i]->mesh);
        }
    }
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // SHADOW PASS END

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->defferedFbo.fboHandle);
    glViewport(0, 0, (GLsizei) state->windowWidth, (GLsizei)state->windowHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint prevProgram = 0xFFFFFFFF;
    GLuint prog;
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            prog = state->entities[i]->material.shader->program;
            if(prog != prevProgram)
            {
                glUseProgram(state->entities[i]->material.shader->program);
                if(state->entities[i]->material.shader->type == ST_Surface)
                {
                    Vec4 ldir = vec4(0.866f, 0.5f,0.0f,0.0f);
                    Vec4 tempLightDir;
                    mat4Vec4Mul(&tempLightDir, &state->main_cam.transformMatrix, &ldir);
                    glUniform4fv(state->game.texShader.surface.lightDirUnif, 1, (const GLfloat*)&tempLightDir);
                }
                else if(state->entities[i]->material.shader->type == ST_Skydome)
                {
                    modelMatrix = calculateModelMatrix(&state->entities[i]->transform);
                    mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
                }
            }
            modelMatrix = calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);

            glUniformMatrix4fv(state->game.texShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);

            glUniform1i(state->entities[i]->material.shader->surface.diffuseTexture, 0);
            glUniform1i(state->entities[i]->material.shader->surface.normalTexture, 1);

            for(int textureId = 0; textureId < state->entities[i]->material.numTextures; textureId++)
            {
                glActiveTexture(GL_TEXTURE0+textureId);
                glBindTexture(GL_TEXTURE_2D, state->entities[i]->material.texture_handle[textureId]);
            }
            renderMesh(&state->entities[i]->mesh);
            prevProgram = prog;
        }
    }

    drawDebugLines(state->game.lineShader, &state->main_cam);

    glBindVertexArray(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, state->defferedFbo.fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // postprocessing quad
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo_fbo_vertices);
    glVertexAttribPointer(
                0,                  // attribute
                2,                  // number of elements per vertex, here (x,y)
                GL_FLOAT,           // the type of each element
                GL_FALSE,           // take our values as-is
                0,                  // no extra data between each position
                0                   // offset of first element
                );

    // light passes

    // directional light
    glUseProgram(state->postProcShader.program);
    glUniform1i(state->postProcShader.postproc.albedo_unif, 0);
    glUniform1i(state->postProcShader.postproc.depth_unif,  1);
    glUniform1i(state->postProcShader.postproc.normal_unif, 2);
    glUniform1i(state->postProcShader.postproc.position_unif, 3);
    glUniform1i(state->postProcShader.postproc.shadow_unif, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.diffuseTexture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.depthTexture);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.normalTexture);
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.positionTexture);
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, state->shadowmap_fbo.depthFBO.depthTexture);
    glEnableVertexAttribArray(0); // TODO

    // TODO
    Vec4 transformedSunDir;
    mat4Vec4Mul(&transformedSunDir, &state->main_cam.transformMatrix, &state->game.sunDir);
    glUniform4fv(state->postProcShader.postproc.light_unif, 1, (const GLfloat*)&transformedSunDir);
    glUniformMatrix4fv(state->postProcShader.postproc.lightProjM_unif, 1, GL_FALSE, (const GLfloat*)&posToShadowSpace);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // point light
    glUseProgram(state->postProcPointShader.program);
    glUniform1i(state->postProcPointShader.postproc.albedo_unif, 0);
    glUniform1i(state->postProcPointShader.postproc.depth_unif,  1);
    glUniform1i(state->postProcPointShader.postproc.normal_unif, 2);
    glUniform1i(state->postProcPointShader.postproc.position_unif, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.diffuseTexture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.depthTexture);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.normalTexture);
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, state->defferedFbo.deferredFBO.positionTexture);
    glEnableVertexAttribArray(0);
    //Vec4 pointLight(state->main_cam.transformMatrix*Vec4(frames*0.01f,0.7f,frames*0.01f,1.0f));
    //glUniform4fv(postProcPointShader.postproc.light_unif, 1, (const GLfloat*)&pointLight);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);0

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);

    //SKY
    float outerR = 20000.0f;
    float innerR = 18000.0f;
    float outerR2 = outerR*outerR;
    float innerR2 = innerR*innerR;
    float mieMult = 0.005f;
    float rayMult = 0.003f;
    setPosition(&state->game.dome.transform, vec3(state->main_cam.position.x,-innerR/100.f+state->main_cam.position.y,state->main_cam.position.z));
    float cheight = innerR+100.0f;
    updateScaleMatrix(&state->game.dome.transform);
    glUseProgram(state->game.skyShader.program);

    modelMatrix = calculateModelMatrix(&state->game.dome.transform);
    mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
    Mat4 domeScale = scale( vec3(20000.f,20000.f,20000.f));
    glUniformMatrix4fv( state->game.skyShader.skydome.scaleMatrix, 1, GL_FALSE, (const GLfloat*)&domeScale);
    glUniformMatrix4fv( state->game.skyShader.skydome.perspectiveMatrix, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    glUniformMatrix4fv( state->game.skyShader.skydome.transformMatrix,1,GL_FALSE, (const GLfloat*) &modelMatrix);
    Vec3 camPos = vec3(0.0f,innerR,0.0f);
    glUniform3fv(       state->game.skyShader.skydome.v3CameraPos          ,1,(const GLfloat*)&camPos);
    glUniform3fv(       state->game.skyShader.skydome.v3LightDir     		,1,(const GLfloat*)&state->game.sunDir);
    glUniform3f(        state->game.skyShader.skydome.v3InvWavelength, 1.0 / pow(0.650, 4.0), 1.0 / pow(0.570, 4.0), 1.0 / pow(0.475, 4.0));
    glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight  	    , cheight);
    glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight2        , (float)pow(cheight,2.0));
    glUniform1fARB(        state->game.skyShader.skydome.fOuterRadius    	    , outerR);
    glUniform1fARB(        state->game.skyShader.skydome.fOuterRadius2         , outerR2);
    glUniform1fARB(        state->game.skyShader.skydome.fInnerRadius          , innerR);
    glUniform1fARB(        state->game.skyShader.skydome.fInnerRadius2         , innerR2);
    glUniform1fARB(        state->game.skyShader.skydome.fKrESun     	        , rayMult*15.0f);//
    glUniform1fARB(        state->game.skyShader.skydome.fKmESun               , mieMult*15.0f);//
    glUniform1fARB(        state->game.skyShader.skydome.fKr4PI                , rayMult*4.0f*3.142f);
    glUniform1fARB(        state->game.skyShader.skydome.fKm4PI                , mieMult*4.0f*3.142f);
    glUniform1fARB(        state->game.skyShader.skydome.fScale                , 1.f / (outerR - innerR));//
    glUniform1fARB(        state->game.skyShader.skydome.fScaleOverScaleDepth  , (1.0f / (outerR - innerR)) / 0.25f);//
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    renderMesh(&state->game.dome.mesh);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void forwardRender(Permanent_Storage *state, Input *input, float dt)
{
    printf("ftime: %f \n", dt);
    //setPosition(&state->game.testBarrel.transform,state->game.testBarrel.transform.position+ vec3(0.01f,0.0f,0.01f));

    glDisable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);

    Mat4 modelMatrix;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state->zprepass_fbo.fboHandle);
    //glViewport(0, 0, (GLsizei) SHADOWMAP_RES, (GLsizei) SHADOWMAP_RES);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Z PREPASS
    glUseProgram(state->shadowPassShader.program);
    glUniformMatrix4fv(state->shadowPassShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            modelMatrix = calculateModelMatrix(&state->entities[i]->transform);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->shadowPassShader.surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
            renderMesh(&state->entities[i]->mesh);
        }
    }
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // LIGHT CULLING

    glUseProgram(state->lightCullShader.program);
    glUniformMatrix4fv(state->lightCullShader.lightc.projection, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
    glUniformMatrix4fv(state->lightCullShader.lightc.view, 1, GL_FALSE, (const GLfloat*) &state->main_cam.transformMatrix);

    glUniform1i(state->lightCullShader.lightc.lightCount, NUM_LIGHTS);
    glUniform2i(state->lightCullShader.lightc.screenSize, state->windowWidth, state->windowHeight);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(state->lightCullShader.lightc.depthMap, 4);
    glBindTexture(GL_TEXTURE_2D, state->zprepass_fbo.depthFBO.depthTexture);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->fplus.lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->fplus.visibleLightIndicesBuffer);
    glDispatchCompute(state->fplus.workGroupsX, state->fplus.workGroupsY, 1);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    // RENDER

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->fplus.lightBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->fplus.visibleLightIndicesBuffer);

    GLuint prevProgram = 0xFFFFFFFF;
    GLuint prog;
    for(int i = 0; i < state->numEntities; i++)
    {
        if(state->entities[i]->visible)
        {
            prog = state->entities[i]->material.shader->program;
            if(prog != prevProgram)
            {
                glUseProgram(state->entities[i]->material.shader->program);

                if(state->entities[i]->material.shader->type == ST_Surface)
                {
                    glUniformMatrix4fv(state->entities[i]->material.shader->surface.viewMatixUnif, 1, GL_FALSE, (const GLfloat*) &state->main_cam.transformMatrix);
                }
            }
            modelMatrix = calculateModelMatrix(&state->entities[i]->transform);
            //printf("mmatrix: %d tf: %d %d\n",state->entities[i]->material.shader->surface.modelMatrix,state->entities[i]->material.shader->program, state->game.texShaderForw.program);
            glUniformMatrix4fv(state->entities[i]->material.shader->surface.modelMatrix, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
            mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
            glUniformMatrix4fv(state->entities[i]->material.shader->surface.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*) &modelMatrix);
            glUniform3fv(state->entities[i]->material.shader->surface.cameraPosition, 1, (const GLfloat*) &state->main_cam.position);

            glUniform1i(state->entities[i]->material.shader->surface.diffuseTexture, 0);
            glUniform1i(state->entities[i]->material.shader->surface.normalTexture, 1);

            for(int textureId = 0; textureId < state->entities[i]->material.numTextures; textureId++)
            {
                glActiveTexture(GL_TEXTURE0+textureId);
                glBindTexture(GL_TEXTURE_2D, state->entities[i]->material.texture_handle[textureId]);
            }
            renderMesh(&state->entities[i]->mesh);
            prevProgram = prog;
        }
    }
}

r32 timeSinceStart;
void display(EngineMemory *mem, Input *input, float dt)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    timeSinceStart += dt;


    float camSpeed = 10.f;
    Vec3 forward = cameraCalculateForwardDirection(&state->main_cam);
    Vec3 right = cameraCalculateRightDirection(&state->main_cam);

    r32 rspeed = dt*camSpeed;

    if(getKey(input, KEYCODE_W))
    {
        Vec3 scaledForw;
        vec3Scale(&scaledForw, &forward, rspeed);
        vec3Add(&state->main_cam.position,&state->main_cam.position,&scaledForw);
        //state->main_cam.position += forward*dt*camSpeed;
    }
    if(getKey(input, KEYCODE_S))
    {
        Vec3 scaledForw;
        vec3Scale(&scaledForw, &forward, rspeed);
        vec3Sub(&state->main_cam.position,&state->main_cam.position,&scaledForw);
    }
    if(getKey(input, KEYCODE_A))
    {
        Vec3 scaledRight;
        vec3Scale(&scaledRight, &right, rspeed);
        vec3Sub(&state->main_cam.position,&state->main_cam.position,&scaledRight);
    }
    if(getKey(input, KEYCODE_D))
    {
        Vec3 scaledRight;
        vec3Scale(&scaledRight, &right, rspeed);
        vec3Add(&state->main_cam.position,&state->main_cam.position,&scaledRight);
    }

    Quaternion movX = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)-input->mousePosition.x*0.4f)*PI/180.f);
    Quaternion movY = quaternionFromAxisAngle( vec3(1.f,0.f,0.f),((float)-input->mousePosition.y*0.4f-300.0f)*PI/180.f);
    quaternionMul(&state->main_cam.rotation, &movX, &movY);

    cameraRecalculateMatrices(&state->main_cam);

    Quaternion sr = quaternionFromAxisAngle( vec3(1.0f,0.0f,0.0f),0.785f);
    Quaternion sr2 = quaternionFromAxisAngle( vec3(1.0f,0.0f,0.0f),3.14+0.785f);

    Vec4 sunDir= vec4(0.0,0.0,-1.0,0.0);
    Mat4 sunRot;
    mat4FromQuaternion(&sunRot, &sr);
    mat4FromQuaternion(&state->game.sunRot2, &sr2);
    state->game.sunRot2Quat = sr2;
    Vec4 resRot;
    mat4Vec4Mul(&resRot, &sunRot, &sunDir);
    sunDir = resRot;
    state->game.sunDir = sunDir;

    state->game.wall.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)frames/200.0f));
    state->game.wall2.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)frames/200.0f));
    state->game.wall.transform.dirty = true;
    state->game.wall2.transform.dirty = true;
    setListenerTransform(state->main_cam.position, cameraCalculateForwardDirection(&state->main_cam), cameraCalculateUpDirection(&state->main_cam));

    if(getKeyDown(input, KEYCODE_P))
    {
        //printf("texShader: %d\n",state->game.texShader.program);
        //printf("texShaderForw: %d\n",state->game.texShaderForw.program);
        state->deferred = !state->deferred;
        if(state->deferred)
        {
            for(int i = 0; i < state->numEntities; i++)
            {
                state->entities[i]->material.shader = &state->game.texShader;
            }
        }
        else
        {
            for(int i = 0; i < state->numEntities; i++)
            {
                state->entities[i]->material.shader = &state->game.texShaderForw;
            }
        }
    }

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepth(1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    visibilityCheck(state); // frustum culling
    if(state->deferred)
    {
        deferredRender(state, input, dt);
    }
    else
    {
        forwardRender(state, input, dt);

        glUseProgram(state->game.particleShader.program);
        glUniformMatrix4fv(state->game.particleShader.terrainGen.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
        glUniformMatrix4fv(state->game.particleShader.terrainGen.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
        glUniformMatrix4fv(state->game.particleShader.terrainGen.viewMatixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
        //glUniformVector3fv(state->game.particleShader.terrainGen.ca
        //glUniform1i(state->postProcPointShader.postproc.position_unif, 5);
        //int tex = state->game.particleShader.surface.diffuseTexture;
        // TODO: make a sparate uniform location for this
        glUniform1i(state->game.particleShader.terrainGen.mcubesTexture1, 0);
        glUniform1i(state->game.particleShader.terrainGen.diffuseTexture, 1);
        glUniform1i(state->game.particleShader.terrainGen.mcubesTexture2, 2);
        //printf("mcubes2 %d \n",state->game.particleShader.terrainGen.mcubesTexture2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, state->mcubesTexture);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, state->zprepass_fbo.depthFBO.depthTexture);
        glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_1D, state->mcubesTexture2);

        //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, state->mcubesBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->fplus.lightBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->fplus.visibleLightIndicesBuffer);

        //printf("tfb error1: %d\n",glGetError());
        if(state->captured == 0)
        {
            GLuint query;
            glGenQueries(1, &query);

            //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, state->tfeedback);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, state->tfeedbackBuffer);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, state->tfeedbackBuffer);
            glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
            glBeginTransformFeedback(GL_TRIANGLES);

            renderAndUpdateParticleSystem(&state->game.particles,dt);

            glEndTransformFeedback();
            state->captured = true;
            //printf("tfb error2: %d\n",glGetError());

            glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
            GLuint primitives;
            glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
            state->numberOfTriangles = primitives;
            printf("%u primitives written!\n\n", primitives); // 36605


            glFinish();
            /*GLfloat feedback[66];
            glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
            for (int i = 0; i < 66; i++) {
                printf("%f\n", feedback[i]);
            }*/
            //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        }
        else
        {
            glUseProgram(state->game.straightShader.program);

            glUniformMatrix4fv(state->game.straightShader.surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
            glUniformMatrix4fv(state->game.straightShader.surface.viewMatixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
            glUniform3fv(state->game.straightShader.surface.cameraPosition, 1, (const GLfloat*) &state->main_cam.position);

            Vec4 ldir = vec4(0.866f, 0.5f,0.0f,0.0f);
            //Vec4 tempLightDir;
            //mat4Vec4Mul(&tempLightDir, &state->main_cam.transformMatrix, &ldir);
            glUniform4fv(state->game.straightShader.surface.lightDirUnif, 1, (const GLfloat*)&ldir);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, state->tfeedbackBuffer);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 40, (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 40, (void*)16);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 40, (void*)28);
            glDrawArrays(GL_TRIANGLES, 0, state->numberOfTriangles*3); /* TODO: according to docs this causes sync problems with feedback buffer*/
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    //updateLights(state, sin(timeSinceStart)*0.5f+0.76f);
    frames++;
}

void reshape (EngineMemory *mem, int w, int h)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    openglDeleteFbo(&state->defferedFbo);
    openglCreateDeferredFBO(&state->defferedFbo,w,h);

    if(state->windowWidth == w && state->windowHeight == h)
        return;
    state->windowWidth = w;
    state->windowHeight = h;

    state->main_cam.aspectRatio = (r32)w/(r32)h;
    cameraRecalculateMatrices(&state->main_cam);

    for(u32 i = 0; i < state->numShaders; i++)
    {
        if(state->shaders[i]->type != ST_Surface)
            continue;
        glUseProgram(state->shaders[i]->program);
        glUniformMatrix4fv(state->shaders[i]->surface.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
        glUseProgram(0);
    }

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void gameExit(EngineMemory *game_memory)
{
    audioExit();
}
