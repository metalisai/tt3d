#include "core.h"

#include <stdio.h>
//#include <math.h>
#include <stdio.h>
#include <string.h>
#include "opencl.h"

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

void initMCubesBuffer2(Permanent_Storage *state)
{
    state->mcubesTexture = opengl_Int16Texture2D(16,256,a2iTriangleConnectionTable2);
    state->mcubesTexture2 = opengl_Int16Texture1D(256, aiCubeEdgeFlags2);
}

Vec3 getChunkOrigin(IVec3 chunkId)
{
    Vec3 ret;
    ret.x = chunkId.x/* * voxelSize*/ * CHUNK_SIZE;
    ret.y = 0.0f;
    ret.z = chunkId.z/* * voxelSize*/ * CHUNK_SIZE;
    return ret;
}

IVec3 getChunkId(Vec3 position)
{
    IVec3 ret;
    ret.x = (i32)floorf(((r32)position.x / (/*voxelSize**/CHUNK_SIZE)));
    ret.y = 0;
    ret.z = (i32)floorf(((r32)position.z / (/*voxelSize**/CHUNK_SIZE)));
    return ret;
}

/*TerrainChunk* getChunksPointer(Permanent_Storage *state, u32 lod)
{
    TerrainChunk* chunks;
    switch(lod)
    {
        case 3:
        {
            chunks = state->game.lod3LoadedChunks;
        } break;
        case 2:
        {
            chunks = state->game.lod2LoadedChunks;
        } break;
        case 1:
        {
            chunks = state->game.lod1LoadedChunks;
        } break;
        default:
            assert(false);
            break;
    }
    return chunks;
}*/

b32 isChunkLoaded(Permanent_Storage *state, IVec3 chunkId)
{
    TerrainChunk* chunks = state->game.loadedChunks;
    for(u32 i = 0; i < state->game.totalLoadedChunkCount; i++)
    {
        if(chunks[i].chunkCoordinate.x == chunkId.x &&
                chunks[i].chunkCoordinate.y == chunkId.y &&
                chunks[i].chunkCoordinate.z == chunkId.z)
        {
            return chunks[i].isAllocate;
        }
    }
    return 0;
}

u32 getHighestChunkRing(Permanent_Storage *state, Camera* cam)
{
    IVec3 currentChunk = getChunkId(cam->position);

    i32 maxRing = -1;

    TerrainChunk* chunks = state->game.loadedChunks;
    for(u32 i = 0; i < state->game.totalLoadedChunkCount; i++)
    {
        i32 ringx = abs(chunks[i].chunkCoordinate.x - currentChunk.x);
        i32 ringz = abs(chunks[i].chunkCoordinate.z - currentChunk.z);
        i32 maxValue = max(ringx,ringz);
        if(maxRing < maxValue)
            maxRing = maxValue;
    }

    assert(maxRing >= 1);
    return maxRing;
}

r32 calculatePriority(Vec3 *pos, Camera *cam)
{
    float distanceWeight = 250.0f;
    Vec3 camToChunk;
    vec3Sub(&camToChunk, pos, &cam->position);
    r32 priorityValue = distanceWeight / vec3Mag2(&camToChunk);
    /*if(cameraIsPointInFrustum(cam, prioritiOrigin))
        priorityValue *= frustumMultiplier;*/
    return priorityValue;
}

struct ChunkPriorityResult
{
    r32 highestPriority;
    TerrainChunk* highestPriorityChunk;
    r32 lowestPriority;
    TerrainChunk* lowestPriorityChunk;
} ChunkPriorityResult;

void getLodPriorityExtremes(Permanent_Storage *state, Camera* cam, struct ChunkPriorityResult* result, u32 lod)
{
    //r32 voxelScale = state->terrainGenState.voxelScale;
    Vec3 toMiddle = vec3(CHUNK_SIZE/2,0.0f,CHUNK_SIZE/2);

    r32 minPriority = R32MAX;
    r32 maxPriority = R32MIN;
    TerrainChunk* lowestChunk = 0;
    TerrainChunk* highestChunk = 0;

    TerrainChunk* chunks = state->game.loadedChunks;
    for(u32 i = 0; i < state->game.totalLoadedChunkCount; i++)
    {
        if(chunks[i].isAllocate && chunks[i].LODLevel == lod)
        {
            Vec3 prioritiOrigin;
            vec3Add(&prioritiOrigin, &chunks[i].origin, &toMiddle);

            r32 priorityValue = calculatePriority(&prioritiOrigin,cam);
            if(minPriority > priorityValue)
            {
                minPriority = priorityValue;
                lowestChunk = &chunks[i];
            }
            if(maxPriority < priorityValue)
            {
                maxPriority = priorityValue;
                highestChunk = &chunks[i];
            }
        }
    }
    result->lowestPriority = minPriority;
    result->lowestPriorityChunk = lowestChunk;
    result->highestPriority = maxPriority;
    result->highestPriorityChunk = highestChunk;
}

void chunkCheck(Permanent_Storage *state, Camera *cam)
{
    IVec3 currentChunk = getChunkId(cam->position);

    if(state->game.loadedChunkCount[3] < MAX_LOD_3_LOADED_CHUNKS ||
            state->game.loadedChunkCount[2] < MAX_LOD_2_LOADED_CHUNKS ||
            state->game.loadedChunkCount[1] < MAX_LOD_1_LOADED_CHUNKS)
    {
        i32 searchRing = 1;
        while(state->game.loadedChunkCount[3] < MAX_LOD_3_LOADED_CHUNKS ||
              state->game.loadedChunkCount[2] < MAX_LOD_2_LOADED_CHUNKS ||
              state->game.loadedChunkCount[1] < MAX_LOD_1_LOADED_CHUNKS)
        {
            for(int i = -searchRing; i <= searchRing; i++)
            {
                for(int j = -searchRing; j <= searchRing; j++)
                {
                    if(state->game.loadedChunkCount[3] < MAX_LOD_3_LOADED_CHUNKS)
                    {
                        IVec3 searchChunk = currentChunk;
                        searchChunk.x += i;
                        searchChunk.z += j;
                        if(!isChunkLoaded(state, searchChunk))
                        {
                            TerrainChunk* ch = &state->game.loadedChunks[state->game.totalLoadedChunkCount++];
                            state->game.loadedChunkCount[3]++;
                            *ch = loadChunk(state, searchChunk, 3);
                            addEntity(state, &ch->entity);
                            printf("load a chunk3 %d %d %d\n",searchChunk.x,searchChunk.y,searchChunk.z);
                            return;
                        }
                    }
                    else if(state->game.loadedChunkCount[2] < MAX_LOD_2_LOADED_CHUNKS)
                    {
                        IVec3 searchChunk = currentChunk;
                        searchChunk.x += i;
                        searchChunk.z += j;
                        if(!isChunkLoaded(state, searchChunk))
                        {
                            TerrainChunk* ch = &state->game.loadedChunks[state->game.totalLoadedChunkCount++];
                            state->game.loadedChunkCount[2]++;
                            *ch = loadChunk(state, searchChunk, 2);
                            addEntity(state, &ch->entity);
                            printf("load a chunk2 %d %d %d\n",searchChunk.x,searchChunk.y,searchChunk.z);
                            return;
                        }
                    }
                    else if(state->game.loadedChunkCount[1] < MAX_LOD_1_LOADED_CHUNKS)
                    {
                        IVec3 searchChunk = currentChunk;
                        searchChunk.x += i;
                        searchChunk.z += j;
                        if(!isChunkLoaded(state, searchChunk))
                        {
                            TerrainChunk* ch = &state->game.loadedChunks[state->game.totalLoadedChunkCount++];
                            state->game.loadedChunkCount[1]++;
                            *ch = loadChunk(state, searchChunk, 1);
                            addEntity(state, &ch->entity);
                            printf("load a chunk1 %d %d %d\n",searchChunk.x,searchChunk.y,searchChunk.z);
                            return;
                        }
                    }
                    else
                    {
                        goto stop;
                    }
                }
            }
            stop:
            searchRing++;
        }
    }
    else
    {
        Vec3 toMiddle = vec3(CHUNK_SIZE/2,0.0f,CHUNK_SIZE/2);

        r32 maxUnloadedPriority = R32MIN;
        IVec3 highestUnloadedChunkId;

        // find highest unloaded chunkId
        i32 ring = getHighestChunkRing(state, cam);
        for(i32 i = -ring; i <= ring; i++)
        {
            for(i32 j = -ring; j <= ring; j++)
            {
                IVec3 searchChunk = currentChunk;
                searchChunk.x += i;
                searchChunk.z += j;
                if(!isChunkLoaded(state, searchChunk))
                {
                    Vec3 chunkOrigin = getChunkOrigin(searchChunk);
                    vec3Add(&chunkOrigin, &chunkOrigin, &toMiddle);

                    r32 priorityValue = calculatePriority(&chunkOrigin, cam);
                    if(maxUnloadedPriority < priorityValue)
                    {
                        maxUnloadedPriority = priorityValue;
                        highestUnloadedChunkId = searchChunk;
                    }
                }
            }
        }

        struct ChunkPriorityResult result[4] = {};
        for(int lod = 1; lod <= 3; lod++)
        {
            getLodPriorityExtremes(state, cam, &result[lod], lod);
            //printf("lod %d max %f min %f \n",lod, result[lod].highestPriority, result[lod].lowestPriority);
        }

        if(result[1].lowestPriority < maxUnloadedPriority)
        {
            Vec3 newOrigin = getChunkOrigin(highestUnloadedChunkId);
            reloadChunk(state, newOrigin, result[1].lowestPriorityChunk, 1);
            result[1].lowestPriorityChunk->origin = newOrigin;
            result[1].lowestPriorityChunk->chunkCoordinate = highestUnloadedChunkId;
            printf("changed chunk to %d %d %d\n",highestUnloadedChunkId.x,highestUnloadedChunkId.y,highestUnloadedChunkId.z);
            return;
        }

        for(int lod = 1; lod < 3; lod++)
        {
            if(result[lod].highestPriority > result[lod+1].lowestPriority)
            {
                reloadChunk(state, result[lod].highestPriorityChunk->origin, result[lod].highestPriorityChunk, lod+1);
                reloadChunk(state, result[lod+1].lowestPriorityChunk->origin, result[lod+1].lowestPriorityChunk, lod);
                printf("proted chunk from lod %d to %d\n",lod,lod+1);
                return;
            }
        }
    }
}

PlatformApi Platform;
void init(EngineMemory *mem, int width, int height)
{
    Platform = mem->platformApi;

    //inticl();

    memset(mem->gameState, 0, Megabytes(100));

    TransientStorage* tmem = (TransientStorage*)mem->transientState;
    createArena(&tmem->mainArena, (u8*)tmem+(sizeof(TransientStorage)+sizeof(TransientStorage)%MEMORYARENA_ALIGNMENT), Megabytes(50));

    MemoryArena *textureMem = arenaPushSize(&tmem->mainArena,Megabytes(5)+sizeof(MemoryArena));
    createArena(textureMem, textureMem+1, Megabytes(5));
    tmem->texturePool = createTexturePool(Kilobytes(1), textureMem);

    Camera* mainCam = &tmem->mainCamera;
    mainCam->FOV = 45.0f;
    mainCam->nearPlane = 0.1f;
    mainCam->farPlane = 200.f;
    cameraInitialize(mainCam);

    RenderGroup* renderGroup = &tmem->renderGroup;
    allocateRenderGroup(&tmem->mainArena, renderGroup);
    renderGroup->commands->width = width;
    renderGroup->commands->height = height;
    renderGroup->commands->clearColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

    openglInit(&tmem->glState, width, height);

    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;
    renderGroup->commands->camera = &state->main_cam;
    state->tstorage = tmem;
    state->texturePool = tmem->texturePool;

    cameraInitialize(&(state->main_cam));
    state->main_cam.FOV = 45.f;
    state->main_cam.nearPlane = 0.1f;
    state->main_cam.farPlane = 10000.f;
    state->numEntities = 0;
    state->numShaders = 0;
    state->captured = 0;

    //openglCreateDepthFBO(&state->shadowmap_fbo, SHADOWMAP_RES, SHADOWMAP_RES, true);

    initMCubesBuffer2(state);
    state->game.loadedChunkCount[0] = 0;
    state->game.loadedChunkCount[1] = 0;
    state->game.loadedChunkCount[2] = 0;
    state->game.loadedChunkCount[3] = 0;
    state->game.totalLoadedChunkCount = 0;

    domeMesh = loadMesh("sphere.tt");
    mesh = loadMesh("barra/barra.tt");
    terrainMesh = generateTerrainMesh();

    LoadedTexture *groundTexture = loadTextureToMemory(state->texturePool, "grass.png");
    opengl_LoadTexture(groundTexture);
    /*LoadedTexture *barrelTexture = loadTextureToMemory(state->texturePool, "barra/barra.jpg");
    opengl_LoadTexture(barrelTexture);
    LoadedTexture *stoneTexture = loadTextureToMemory(state->texturePool, "textures/stone.jpg");
    opengl_LoadTexture(stoneTexture);
    LoadedTexture *brickDiffuseTexture = loadTextureToMemory(state->texturePool, "textures/brick_diff.jpg");
    opengl_LoadTexture(brickDiffuseTexture);
    LoadedTexture *brickNormalTexture = loadTextureToMemory(state->texturePool, "textures/brick_norm.jpg");
    opengl_LoadTexture(brickNormalTexture);*/

    Mesh *planeMesh = generatePlane();

    initializeProgram(&state->textShader,"shaders/textvert.glsl", "shaders/textfrag.glsl", 0, ST_Surface);
    initializeProgram(&state->postProcShader,"shaders/postproc_vert.glsl", "shaders/postproc_frag.glsl", 0, ST_PostProc);
    initializeProgram(&state->postProcPointShader,"shaders/postproc_vert.glsl", "shaders/postproc_point_frag.glsl", 0, ST_PostProc);
    initializeProgram(&state->shadowPassShader,"shaders/vert_shadow_pass.glsl", "shaders/frag_shadow_pass.glsl", 0, ST_Surface);
    initializeComputeProgram(&state->lightCullShader, "shaders/forwardp_lightcull.glsl", ST_LightCull);

    initializeProgram(&state->game.notexShader,"shaders/vert.glsl", "shaders/notex_frag.glsl", 0, ST_Surface);
    initializeProgram(&state->game.texShader,"shaders/vert.glsl", "shaders/frag.glsl", 0, ST_Surface);
    initializeProgram(&state->game.skyShader,"shaders/skydomevert.glsl", "shaders/skydomefrag.glsl", 0, ST_Skydome);
    initializeProgram(&state->game.normalShader,"shaders/vert_normal.glsl", "shaders/frag_normal.glsl", 0, ST_Surface);
    initializeProgram(&state->game.lineShader,"shaders/linevert.glsl", "shaders/linefrag.glsl", 0, ST_Surface);
    initializeProgram(&state->game.particleShader,"shaders/particle_vert.glsl", "shaders/frag_color_forward.glsl", "shaders/particle_geo.glsl", ST_Particle);
    //initializeProgram(&state->game.terrainGenShader,"shaders/particle_vert.glsl", "shaders/frag_color_forward.glsl", "shaders/terrain_geo.glsl", ST_Particle);

    i32 res;
    glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,&res);
    printf("shared mem size: %d\n",res);

    int glerror = glGetError();
    if(glerror != 0)
    {
        printf("Error: %d\n", glerror);
        __builtin_trap();
    }

    initializeProgram(&state->game.texShaderForw,"shaders/vert_forward.glsl", "shaders/frag_forward.glsl", 0, ST_Surface);
    initializeProgram(&state->game.straightShader,"shaders/straight_vert.glsl", "shaders/frag_color_forward.glsl", 0, ST_Surface);

    glerror = glGetError();
    if(glerror != 0)
    {
        printf("Error: %d\n", glerror);
        __builtin_trap();
    }

    //initializeComputeProgram(&state->terrainComputeShader, "shaders/terrain_compute.glsl", ST_Particle);
    initializeComputeProgram(&state->terrainComputeShader, "shaders/terrain_compute2.glsl", ST_Particle);

    addSurfaceShader(state, &state->game.notexShader);
    addSurfaceShader(state, &state->game.texShader);
    addSurfaceShader(state, &state->game.normalShader);
    addSurfaceShader(state, &state->game.texShaderForw);

    glerror = glGetError();
    if(glerror != 0)
    {
        printf("Error: %d\n", glerror);
        __builtin_trap();
    }

    meshRecalculateNormals(mesh);
    meshRecalculateNormals(terrainMesh);
    meshRecalculateNormals(planeMesh);
    meshRecalculateNormals(domeMesh);
    openGL_loadMesh(mesh);
    openGL_loadMesh(terrainMesh);
    openGL_loadMesh(planeMesh);
    openGL_loadMesh(domeMesh);

    /*int k = 0;
    for(int i= 0; i < 20; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            state->game.barrel[k].mesh = *mesh;
            transformInit(&state->game.barrel[k].transform);
            state->game.barrel[k].transform.position =  vec3((float)i+10.0f,0.f,(float)j+10.0f);
            state->game.barrel[k].material.numTextures = 1;
            state->game.barrel[k].material.shader = &state->game.texShaderForw;
            state->game.barrel[k].material.texture_handle[0] = barrelTexture->textureHandle;
            addEntity(state, &state->game.barrel[k]);
            k++;
        }
    }*/

    state->game.voxelTerrainCount = 0;
    /*for(int i = 0; i < 4; i++)
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
        state->game.voxelTerrain[i].material.shader = &state->game.texShaderForw;
        state->game.voxelTerrain[i].material.texture_handle[0] = INT2VOIDP(state->shadowmap_fbo.depthFBO.depthTexture);
        //addEntity(state, &state->game.voxelTerrain[i]);
    }*/

    state->game.terrain.mesh = *terrainMesh;
    transformInit(&state->game.terrain.transform);
    state->game.terrain.transform.position =  vec3(0.f,0.f,0.f);
    state->game.terrain.material.numTextures = 1;
    state->game.terrain.material.shader = &state->game.texShaderForw;
    state->game.terrain.material.texture_handle[0] = groundTexture->textureHandle;
    addEntity(state, &state->game.terrain);

    //state->game.testBarrel.mesh = *mesh/*planeMesh*/;
    /*transformInit(&state->game.testBarrel.transform);
    state->game.testBarrel.material.numTextures = 1;
    state->game.testBarrel.material.shader = &state->game.texShaderForw;
    state->game.testBarrel.material.texture_handle[0] = barrelTexture->textureHandle;
    addEntity(state, &state->game.testBarrel);

    state->game.terrain.mesh = *terrainMesh;
    transformInit(&state->game.terrain.transform);
    //printf("scalex %f",state->game.terrain.transform.scale.x);
    state->game.terrain.transform.position =  vec3(0.f,0.f,0.f);
    state->game.terrain.material.numTextures = 1;
    state->game.terrain.material.shader = &state->game.texShaderForw;
    state->game.terrain.material.texture_handle[0] = groundTexture->textureHandle;
    addEntity(state, &state->game.terrain);

    state->game.wall.mesh = *planeMesh;
    transformInit(&state->game.wall.transform);
    state->game.wall.transform.position =  vec3(10.0f,2.0f,10.0f);
    state->game.wall.material.numTextures = 2;
    state->game.wall.material.shader = &state->game.texShaderForw;
    state->game.wall.material.texture_handle[0] = brickDiffuseTexture->textureHandle;
    state->game.wall.material.texture_handle[1] = brickNormalTexture->textureHandle;
    addEntity(state, &state->game.wall);

    state->game.wall2.mesh = *planeMesh;
    transformInit(&state->game.wall2.transform);
    state->game.wall2.transform.position =  vec3(9.0f,2.0f,10.0f);
    state->game.wall2.material.numTextures = 2;
    state->game.wall2.material.shader = &state->game.texShaderForw;
    state->game.wall2.material.texture_handle[0] = brickDiffuseTexture->textureHandle;
    addEntity(state, &state->game.wall2);*/

    state->game.dome.mesh = *domeMesh;
    transformInit(&state->game.dome.transform);
    state->game.dome.transform.position =  vec3(0.0f,0.0f,0.0f);;
    state->game.dome.material.numTextures = 0;
    state->game.dome.material.shader = &state->game.skyShader;
    //dome.transform.scale = Vec3(3500.0f,3500.0f,3500.0f);
    state->game.dome.transform.scale =  vec3(1000.0f,1000.0f,1000.0f);
    //addEntity(&dome);

    if(!glCreateShader(GL_GEOMETRY_SHADER)){
        printf("Creating geos failed\n");
    }

    // Debug stuff
    setupDebug(&state->debugState);
    // TODO: remove constant
    u32 maxGroups = CHUNK_SIZE/CHUNK_WORKGROUP_SIZE;
    openglInitializeTerrainGeneration(&state->terrainGenState, maxGroups, CHUNK_WORKGROUP_SIZE, 4.0);
}

int frames = 0;

void pushVisibleEntities(Permanent_Storage *state)
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

    for(int i = 0; i < state->numEntities; i++)
    {
        r32 bR = state->entities[i]->entityType == 1 ? state->entities[i]->amesh.boundingRadius : state->entities[i]->mesh.boundingRadius;
        // TODO: check for all 6 planes?
        Vec3 pos;
        vec3Sub(&pos, &state->entities[i]->transform.position, &state->main_cam.position);
        r32 res = vec3Dot(&pos, &n[0]) + d[0]+bR*state->entities[i]->transform.scale.x;
        r32 res2 = vec3Dot(&pos, &n[1]) + d[1]+bR*state->entities[i]->transform.scale.x;
        if(res <= 0.f || res2 <= 0.f)
            state->entities[i]->visible = false;
        else
        {
            if(state->entities[i]->entityType != 1)
            {
                pushMesh(&state->tstorage->renderGroup, &state->entities[i]->mesh,
                         &state->entities[i]->transform, state->entities[i]->material);
            }
            else
            {
                pushArrayMesh(&state->tstorage->renderGroup, &state->entities[i]->amesh,
                              &state->entities[i]->transform, state->entities[i]->material);
            }
            state->entities[i]->visible = true;
        }
    }
}

Vec3 lpos1;
//Vec3 lpos2;

Vec3 curve(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, r32 t)
{
    r32 s0,s1,s2,s3;
    s0 = (r32)powf((1-t),3);
    s1 = (r32)(3*powf((1-t),2));
    s2 = (r32)(3*(1-t)*powf(t,2));
    s3 = (r32)(powf(t,3));

    Vec3 v0, v1, v2, v3;
    vec3Scale(&v0, &p0, s0);
    vec3Scale(&v1, &p1, s1);
    vec3Scale(&v2, &p2, s2);
    vec3Scale(&v3, &p3, s3);

    Vec3 result;
    vec3Add(&result, &v0, &v1);
    vec3Add(&result, &result, &v2);
    vec3Add(&result, &result, &v3);
    return result;
}

Vec3 getClosestPointOnLine(Vec3 a, Vec3 b, Vec3 from)
{
    Vec3 atob;
    vec3Sub(&atob, &b, &a);
    vec3Sub(&from, &from, &a);

    r32 atob2 = vec3Mag2(&atob);

    r32 atopDotAtob = vec3Dot(&from, &atob);
    r32 t = atopDotAtob / atob2;

    t = clamp01(t);

    Vec3 res;
    vec3Scale(&atob, &atob, t);
    vec3Add(&res, &a, &atob);

    return res;
}

void forwardRender(Permanent_Storage *state, Input *input, float dt)
{
    printf("frame time: %f (fps: %f)\n",dt,1.0f/dt);

    openGLRenderCommands(&state->tstorage->glState,state->tstorage->renderGroup.commands, state->windowWidth, state->windowHeight);

    drawLine(&state->debugState, vec3(0.0f, 0.0f, 0.0f), lpos1);
    drawDebugLines(&state->debugState, state->game.lineShader, &state->main_cam);

    /*if(getKeyDown(input, KEYCODE_Q))
    {
        lpos1 = screenToWorldPoint(state, vec2(0.0f,0.0f));

        Vec3 wpos = vec3(0.0f,0.0f,0.0f);
        Vec2 scr = worldToScreenPoint(state, wpos);
        Vec2 mp = input->mouseCoord;
        Vec2 result;
        vec2Sub(&result, &scr, &mp);
        //printf("mpos %f %f\n", scr.x, scr.y);
        if(vec2Mag(&result) < 0.1)
        {
            printf("clicked around 0!\n");
        }
    }*/

    //r32 t = 0;

    Vec3 p0, p1, p2, p3;
    p0 = vec3(0.0f,0.0f,0.0f);
    p1 = vec3(-2.0f,0.0f,5.0f);
    p2 = vec3(6.0f,0.0f,2.5f);
    p3 = vec3(5.5f,0.0f,0.0f);

    /*for(r32 i = 0.0f; i < 1.0f; i+=0.1f)
    {
        Vec3 pt0 = curve(p0, p1, p2, p3, i);
        Vec3 pt1 = curve(p0, p1, p2, p3, i+0.1);
        drawLine(&state->debugState, pt0, pt1);
    }*/

    drawLine(&state->debugState, p0, p3);
    Vec3 closest = getClosestPointOnLine(p0,p3,state->main_cam.position);
    Vec3 closest2;
    Vec3 up = vec3(0.0f,1.0f,0.0f);
    vec3Add(&closest2, &closest, &up);
    drawLine(&state->debugState, closest, closest2);

    //Vec3 pt = p0*(pow((1-t),3)) + p1*(3*pow((1-t),2)) + p2*(3*(1-t)*pow(t,2)) + p3*(pow(t,3));
    //glFinish();
}

Entity* genTerrainChunk(Vec3 offset, Permanent_Storage* state, float scale)
{
    printf("begin\n");
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, Megabytes(20), 0, GL_STATIC_READ);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(state->game.particleShader.program);
    glUniformMatrix4fv(state->game.particleShader.terrainGen.perspectiveMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.perspectiveMatrix);
    glUniformMatrix4fv(state->game.particleShader.terrainGen.transformMatrixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
    glUniformMatrix4fv(state->game.particleShader.terrainGen.viewMatixUnif, 1, GL_FALSE, (const GLfloat*)&state->main_cam.transformMatrix);
    glUniform1i(state->game.particleShader.terrainGen.mcubesTexture1, 0);
    glUniform1i(state->game.particleShader.terrainGen.diffuseTexture, 1);
    glUniform1i(state->game.particleShader.terrainGen.mcubesTexture2, 2);
    //printf("wo %d\n",state->game.particleShader.terrainGen.worldOffset);
    glUniform3fv(state->game.particleShader.terrainGen.worldOffset, 1, (GLfloat*)&offset);
    glUniform1f(state->game.particleShader.terrainGen.voxelScale, scale);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->mcubesTexture);
    //glActiveTexture(GL_TEXTURE0+1);
    //glBindTexture(GL_TEXTURE_2D, state->tstorage->glState.zprepass_fbo.depthFBO.depthTexture);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_1D, state->mcubesTexture2);

    GLuint query;
    glGenQueries(1, &query);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, buffer);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
    glBeginTransformFeedback(GL_TRIANGLES);

    glEndTransformFeedback();
    glFinish(); // TODO: is this enough for sync?
    state->captured = true;

    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    GLuint primitives;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
    state->numberOfTriangles = primitives;
    printf("%u primitives written!\n\n", primitives); // 36605

    ArrayMesh mesh;
    mesh.AttribBuffer = buffer;
    mesh.loadedToGPU = true;
    mesh.boundingRadius = R32MAX;
    mesh.data = NULL;
    mesh.faces = primitives;
    mesh.vertices = primitives*3;
    mesh.vertexStride = 28;
    Entity *vt = &state->game.voxelTerrain[state->game.voxelTerrainCount++];
    vt->material.numTextures = 0;
    vt->material.shader = &state->game.straightShader;
    vt->amesh = mesh;
    vt->entityType = 1;
    transformInit(&vt->transform);
    //addEntity(state, vt);

    return vt;
}

#include <time.h>

void reloadChunk(Permanent_Storage* state, Vec3 origin, TerrainChunk* tchunk, u32 lodLevel)
{
    tchunk->LODLevel = lodLevel;
    assert(lodLevel > 0);

    glFinish();
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    ArrayMesh* mesh = &tchunk->entity.amesh;

#if 0
    state->terrainGenState.tunnelData.secondOctaveMax = (r32)absf(origin.z)/(r32)CHUNK_SIZE;
    state->terrainGenState.tunnelData.dxgoalFirstOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dzgoalFirstOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dxgoalSecondOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dzgoalSecondOctaveMax = (absf(origin.z+(r32)CHUNK_SIZE)/(r32)CHUNK_SIZE)-state->terrainGenState.tunnelData.secondOctaveMax;
    state->terrainGenState.tunnelData.chunkOrigin = vec4FromVec3AndW(origin,1.0);
#else
    state->terrainGenState.tunnelData.secondOctaveMax = 30.0f;
    state->terrainGenState.tunnelData.dxgoalFirstOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dzgoalFirstOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dxgoalSecondOctaveMax = 0.0f;
    state->terrainGenState.tunnelData.dzgoalSecondOctaveMax = 0.0f;
#endif

    u32 groups = powInt(2,lodLevel-1);
    r32 scale = (CHUNK_SIZE/CHUNK_WORKGROUP_SIZE)/groups;

    openglPrepageTerrainGeneration(&state->terrainGenState, mesh->AttribBuffer, mesh->ElementBuffer, groups, scale);
    glUseProgram(state->terrainComputeShader.program);
    glUniform1i(state->terrainComputeShader.terrainGen.mcubesTexture1, 0);
    glUniform1i(state->terrainComputeShader.terrainGen.mcubesTexture2, 2);
    glUniform3fv(state->terrainComputeShader.terrainGen.worldOffset, 1, (GLfloat*)&origin);
    glUniform1f(state->terrainComputeShader.terrainGen.voxelScale, scale);
    if(state->terrainComputeShader.terrainGen.mcubesTexture1 == -1 /*|| state->terrainComputeShader.terrainGen.mcubesTexture2 == -1*/
            || state->terrainComputeShader.terrainGen.worldOffset == -1 || state->terrainComputeShader.terrainGen.voxelScale == -1)
    {
        assert(false);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->mcubesTexture);
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_1D, state->mcubesTexture2);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->terrainGenState.vertInbuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->AttribBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, state->terrainGenState.tunnelBuffer);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, state->terrainGenState.vertAtomicBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, state->terrainGenState.edgeVertexBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, mesh->ElementBuffer);

    glDispatchCompute(groups*groups*groups, 1, 1);

    glFinish();
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    /*void* evb = malloc(state->terrainGenState.edgeVertexBufferSize);
    glBindBuffer(GL_ARRAY_BUFFER, state->terrainGenState.edgeVertexBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, state->terrainGenState.edgeVertexBufferSize, evb);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    printf("%p \n",evb);
    free(evb);*/

    GLuint *userCounters;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, state->terrainGenState.vertAtomicBuffer);
    userCounters = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
                                             0,
                                             sizeof(GLuint)*2,
                                             GL_MAP_READ_BIT
                                            );
    u32 triangleCount = userCounters[0];
    u32 indexCount = userCounters[1];
    printf("chunk vertices %d\n", triangleCount);
    printf("chunk index triangles %d\n", indexCount);
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    mesh->vertices = triangleCount;
    mesh->faces = indexCount;
    mesh->loadedToGPU = true;
    mesh->data = NULL;
    mesh->boundingRadius = sqrtf(3*CHUNK_SIZE*CHUNK_SIZE);
    //mesh->boundingRadius = R32MAX;
    mesh->vertexStride = 32;

    /*void* evb = malloc(indexCount*4*3);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->ElementBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, indexCount*4*3, evb);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    for(int i=0; i < indexCount*3; i+=3)
    {
        printf("%d %d %d \n",((int*)evb)[i],((int*)evb)[i+1],((int*)evb)[i+2]);
    }
    free(evb);*/

    /*int size = triangleCount*sizeof(Vec4);
    void* vvb = malloc(size);
    Vec4* vertd = (Vec4*)vvb;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, vvb);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    for(int i=0; i < triangleCount; i++)
    {
        printf("%f %f %f %f \n",vertd[i].x,vertd[i].y,vertd[i].z,vertd[i].w);
    }
    free(vvb);*/

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->AttribBuffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 32, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, (GLvoid*)16);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ElementBuffer);
    glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glFinish();
    struct timespec last;
    clock_gettime(CLOCK_MONOTONIC, &last);
    float ms = (last.tv_sec-start.tv_sec)*1000.0f+(last.tv_nsec-start.tv_nsec)/1000000.0;
    printf("Terrain gen time: %fms\n (LOD: %d)", ms, lodLevel);
    //addEntity(state, chunk2 );*/
    u32 difFromHighest = 4-lodLevel;
    Vec3 offset = vec3(0.0f,-0.2f*difFromHighest,0.0f);
    vec3Add(&origin, &offset, &origin);
    setPosition(&tchunk->entity.transform, origin);
}

TerrainChunk loadChunk(Permanent_Storage* state, IVec3 chunkId, u32 lodLevel)
{
    TerrainChunk ret;
    ret.chunkCoordinate = chunkId;
    ret.isAllocate = 1;
    ret.origin = getChunkOrigin(chunkId);
    ret.LODLevel = lodLevel;

    GLuint outBuffer;
    glGenBuffers(1, &outBuffer);
    GLuint outElBuffer;
    glGenBuffers(1, &outElBuffer);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    ArrayMesh mesh;
    mesh.AttribBuffer = outBuffer;
    mesh.ElementBuffer = outElBuffer;
    mesh.VAO = VAO;

    Entity *vt = &ret.entity;
    vt->material.numTextures = 0;
    vt->material.shader = &state->game.straightShader;
    vt->amesh = mesh;
    vt->entityType = 1;
    transformInit(&vt->transform);

    reloadChunk(state, ret.origin, &ret, lodLevel);

    return ret;
}

r32 timeSinceStart;
void display(EngineMemory *mem, Input *input, float dt)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    chunkCheck(state, &state->main_cam);
    //findHighestPriorityChunk(state, &state->main_cam);

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

    /*Vec3 vector = vec3(0.0,0.316,0.9486);
    Vec3 res = vec3Normalized(&vector);
    printf("VEC %f, %f, %f\n",res.x,res.y,res.z);*/

    state->game.sunDir = vec4(0.0f,0.316f,0.9486f, 0.67f);

    state->game.wall.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)frames/200.0f));
    state->game.wall2.transform.rotation = quaternionFromAxisAngle( vec3(0.f,1.f,0.f),((float)300.0f));
    state->game.wall.transform.dirty = true;
    state->game.wall2.transform.dirty = true;
    setListenerTransform(state->main_cam.position, cameraCalculateForwardDirection(&state->main_cam), cameraCalculateUpDirection(&state->main_cam));

    pushVisibleEntities(state); // frustum culling

    static float distanceFC = 5.0f;
    Camera* cam = &state->main_cam;
    Vec3 bpos;
    Vec3 by = cameraCalculateForwardDirection(cam);
    vec3Scale(&by, &by, distanceFC);
    //vec3Add(&bpos, &cam->position, &by);
    // TODO: apparently super slow
    //r32 depth = openglGetScreenDepth(&state->tstorage->glState, vec2(0.0f,0.0f));
    r32 depth = 1.0f;
    depth *= 2.0f;
    depth -= 1.0f;
    //depth = cameraLinearizeDepth(cam, depth);
    //printf("Depth %f\n",depth);
    bpos = cameraScreenDepthToWorldPoint(&state->main_cam, vec2(0.0f,0.0f), depth);
    Vec3 added;
    Vec3 up = vec3(0.07f,1.0f,0.0f);
    vec3Add(&added, &bpos, &up);

    drawLine(&state->debugState,bpos,added);

    //setPosition(&state->game.barrel[0].transform, bpos);

    if(getKey(input, KEYCODE_R))
    {
        distanceFC += 5.0f*dt;
    }
    if(getKey(input, KEYCODE_T))
    {
        distanceFC -= 5.0f*dt;
    }

    static b32 startedLine;
    static Vec3 startPos;

    if(startedLine)
    {
        drawLine(&state->debugState, startPos, bpos);
    }

    if(getKeyDown(input, KEYCODE_Q))
    {
        if(!startedLine)
        {
            startedLine = 1;
            startPos = bpos;
        }
        else
        {
            int newlineid = state->terrainGenState.tunnelData.tunnelCount++;
            state->terrainGenState.tunnelData.tunnels[newlineid].start = vec4FromVec3AndW(startPos, 1.0f);
            state->terrainGenState.tunnelData.tunnels[newlineid].end = vec4FromVec3AndW(bpos, 1.0f);
            //genComputeTerrain(state, state->game.voxelTerrain[0].amesh.AttribBuffer, false);
            startedLine = 0;
        }
    }

    if(getKeyDown(input, KEYCODE_N))
    {
        state->tstorage->glState.night = !state->tstorage->glState.night;
    }

    {
        forwardRender(state, input, dt);

        if(state->captured == 0)
        {
            /*glFinish();
            struct timespec start;
            clock_gettime(CLOCK_MONOTONIC, &start);

            Entity* chunk = genTerrainChunk(vec3(0.0f,0.0f,0.0f), state, 1.0);
            //Entity* chunk2 = genTerrainChunk(vec3(0.0f,0.0f,0.0f), state, 0.5);
            addEntity(state, chunk);

            glFinish();
            struct timespec last;
            clock_gettime(CLOCK_MONOTONIC, &last);
            float seconds = start.tv_sec-last.tv_sec+(start.tv_nsec-last.tv_nsec)/1000000000.0;
            printf("Terrain gen: %f\n",seconds);
            //addEntity(state, chunk2 );*/

            GLuint terrainBuffer;
            glGenBuffers(1, &terrainBuffer);
            //genComputeTerrain(state, terrainBuffer, true);

            state->captured = 1;
        }

        //SKY
        float outerR = 20000.0f;
        float innerR = 19200.0f;
        float outerR2 = outerR*outerR;
        float innerR2 = innerR*innerR;
        float mieMult = 0.0004f; // original 0.004 (makes sun smaller apparently)
        float rayMult = 0.003f;
        setPosition(&state->game.dome.transform, vec3(state->main_cam.position.x,-innerR/20.f+state->main_cam.position.y,state->main_cam.position.z));
        float cheight = innerR+10.0f;
        updateScaleMatrix(&state->game.dome.transform);
        glUseProgram(state->game.skyShader.program);

        Mat4 modelMatrix = calculateModelMatrix(&state->game.dome.transform);
        mat4Mul(&modelMatrix, &state->main_cam.transformMatrix, &modelMatrix);
        Mat4 domeScale = scale( vec3(20000.f,20000.f,20000.f));
        glUniformMatrix4fv( state->game.skyShader.skydome.scaleMatrix, 1, GL_FALSE, (const GLfloat*)&domeScale);
        glUniformMatrix4fv( state->game.skyShader.skydome.perspectiveMatrix, 1, GL_FALSE, (const GLfloat*) &state->main_cam.perspectiveMatrix);
        glUniformMatrix4fv( state->game.skyShader.skydome.transformMatrix,1,GL_FALSE, (const GLfloat*) &modelMatrix);
        Vec3 camPos = vec3(0.0f,innerR,0.0f);
        glUniform3fv(       state->game.skyShader.skydome.v3CameraPos          ,1,(const GLfloat*)&camPos);
        glUniform3fv(       state->game.skyShader.skydome.v3LightDir     		,1,(const GLfloat*)&state->game.sunDir);
        glUniform3f(        state->game.skyShader.skydome.v3InvWavelength, 1.0 / powf(0.650, 4.0), 1.0 / powf(0.570, 4.0), 1.0 / powf(0.475, 4.0));
        glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight  	    , cheight);
        glUniform1fARB(        state->game.skyShader.skydome.fCameraHeight2        , (float)powf(cheight,2.0));
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
        if(!state->tstorage->glState.night)
            renderMesh(&state->game.dome.mesh);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }
    resetBuffer(&state->tstorage->renderGroup);
    frames++;
}

void reshape (EngineMemory *mem, int w, int h)
{
    Permanent_Storage *state = (Permanent_Storage*)mem->gameState;

    if(state->windowWidth == w && state->windowHeight == h)
        return;
    state->windowWidth = w;
    state->windowHeight = h;

    openglResize(&state->tstorage->glState, w, h);

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
