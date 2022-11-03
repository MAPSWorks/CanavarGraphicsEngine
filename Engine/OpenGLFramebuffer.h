#ifndef OPENGLFRAMEBUFFER_H
#define OPENGLFRAMEBUFFER_H

#include <QOpenGLExtraFunctions>

namespace Canavar {
namespace Engine {

class OpenGLFramebuffer : protected QOpenGLExtraFunctions
{
public:
    OpenGLFramebuffer();

    void init();
    void create(int width, int height);
    void destroy();

    void bind();
    void release();

private:
    bool mCreated;
    unsigned int mFBO;
    unsigned int mRBO;
    unsigned int mTextures[2];

    GLuint mColorAttachments[2];
};

} // namespace Engine
} // namespace Canavar

#endif // OPENGLFRAMEBUFFER_H
