#ifndef COMMON_H
#define COMMON_H

#include <QObject>

#define DEFINE_MEMBER(type, name) \
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

#define DEFINE_MEMBER_CONST(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &get##name(void) const { return m##name; }

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
    PostProcessShader,
    ScreenShader,
    NodeInfoShader,
    NozzleEffectShader,
    FirecrackerEffectShader,
    BasicShader
};

enum class RenderMode { //
    Default = 0x00,
    NodeInfo = 0x01,
    Custom = 0x02,
};

Q_DECLARE_FLAGS(RenderModes, RenderMode);

extern const float CUBE[108];
extern const float QUAD[24];

} // namespace Engine
} // namespace Canavar

Q_DECLARE_OPERATORS_FOR_FLAGS(Canavar::Engine::RenderModes)

#endif // COMMON_H
