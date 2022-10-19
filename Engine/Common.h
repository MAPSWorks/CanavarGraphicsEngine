#ifndef COMMON_H
#define COMMON_H

#include "Node.h"

namespace Canavar {
namespace Engine {

enum class ShaderType { //
    None,
    ModelShader,
};

struct RenderParameters {
    float ifps;
    Node *node;
};

} // namespace Engine
} // namespace Canavar
#endif // COMMON_H
