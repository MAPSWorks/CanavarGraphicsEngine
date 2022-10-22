#ifndef QUAD_H
#define QUAD_H

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class Quad : protected QOpenGLExtraFunctions
{
private:
    Quad();

public:
    void render();

    static Quad *instance();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float VERTICES[24];
};

} // namespace Engine
} // namespace Canavar

#endif // QUAD_H
