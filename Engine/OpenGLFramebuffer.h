#ifndef OPENGLFRAMEBUFFER_H
#define OPENGLFRAMEBUFFER_H

#include <QOpenGLFunctions>

namespace Canavar {
namespace Engine {

class OpenGLFramebuffer : protected QOpenGLFunctions
{
public:
    OpenGLFramebuffer();

    void create(int width, int height);
    void destroy();

    void bind();
    void release();

private:
    bool mCreated;
    unsigned int mFBO;
    unsigned int mRBO;
    unsigned int mTexture;
};

} // namespace Engine
} // namespace Canavar

#endif // OPENGLFRAMEBUFFER_H
