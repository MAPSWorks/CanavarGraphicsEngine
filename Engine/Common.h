#ifndef COMMON_H
#define COMMON_H

#include "Model.h"

namespace Canavar {
namespace Engine {

enum class ShaderType { //
    None,
    ModelColoredShader,
    ModelTexturedShader,
    SkyShader,
    TerrainShader
};

struct RenderParameters {
    float ifps;
    Model *model;
};

} // namespace Engine
} // namespace Canavar
#endif // COMMON_H
