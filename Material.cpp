#include "Material.h"

Material::Material()
    : ambient(0.25f)
    , diffuse(0.75f)
    , specular(0.25f)
    , shininess(32.0f)
    , color(1, 1, 1, 1)
{}
