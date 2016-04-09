#ifndef ENTITY_H
#define ENTITY_H

#include "transform.h"
#include "mesh.h"
#include "material.h"

struct Entity
{
    Transform transform;
    Mesh mesh;
    Material material;
    bool visible;
};

#endif // ENTITY_H
