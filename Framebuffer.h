#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QMap>
#include <QObject>
#include <QOpenGLExtraFunctions>

class Framebuffer : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Framebuffer(int width, int height, QObject *parent = nullptr);
    virtual ~Framebuffer();

    unsigned int texture() const;
    void bind();

private:
    unsigned int mFramebuffer;
    unsigned int mRenderObject;
    unsigned int mTexture;
    int mWidth;
    int mHeight;
};

#endif // FRAMEBUFFER_H
