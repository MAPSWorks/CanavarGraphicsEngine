#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "FramebufferFormat.h"

#include <QMap>
#include <QObject>
#include <QOpenGLExtraFunctions>

class Framebuffer : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Framebuffer(FramebufferFormat format, QObject *parent = nullptr);
    virtual ~Framebuffer();

    void bind();
    unsigned int texture(int index = 0) const;
    unsigned int depth() const;

    const FramebufferFormat &format() const;

private:
    FramebufferFormat mFormat;
    unsigned int mFramebuffer;
    unsigned int mRenderObject;
    QMap<unsigned int, unsigned int> mTextures;
    unsigned int mDepth;
};

#endif // FRAMEBUFFER_H
