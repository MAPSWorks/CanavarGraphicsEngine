#ifndef COMMON_H
#define COMMON_H

#define DECLARE_MEMBER(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &get##name(void) const { return m##name; } \
\
public: \
    inline type &get##name##_nonConst(void) { return m##name; } \
\
public: \
    inline void set##name(const type &var) { m##name = var; }

namespace Canavar {
namespace Engine {

class Model;

enum class ShaderType { //
    None,
    ModelColoredShader,
    ModelTexturedShader,
    SkyShader,
    TerrainShader,
    BlurShader,
    PostProcessShader
};

enum class RenderPass { //
    Default,
    Bloom,
    MeshSelection, // TODO
    NodeSelection, // TODO

};

} // namespace Engine
} // namespace Canavar
#endif // COMMON_H
