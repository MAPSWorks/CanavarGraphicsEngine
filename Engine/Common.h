#ifndef COMMON_H
#define COMMON_H

#include "Node.h"

namespace Canavar {
namespace Engine {

enum class ShaderType { //
    None,
    ModelColoredShader,
    ModelTexturedShader,
};

struct RenderParameters {
    float ifps;
    Node *node;
};

} // namespace Engine
} // namespace Canavar
#endif // COMMON_H
