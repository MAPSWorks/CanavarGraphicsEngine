#include "OpenGLFramebuffer.h"

Canavar::Engine::OpenGLFramebuffer::OpenGLFramebuffer()
    : mCreated(false)
    , mFBO(0)
    , mRBO(0)
    , mTexture(0)
{
    initializeOpenGLFunctions();
}

// TODO: Make this method parametric
void Canavar::Engine::OpenGLFramebuffer::create(int width, int height)
{
    if (mCreated)
    {
        qCritical() << "FBO is already created!";
        return;
    }

    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

    glGenRenderbuffers(1, &mRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qCritical() << "Framebuffer is could not be created.";
        return;
    }

    mCreated = true;
}

void Canavar::Engine::OpenGLFramebuffer::destroy()
{
    if (mFBO)
        glDeleteFramebuffers(1, &mFBO);

    if (mRBO)
        glDeleteBuffers(1, &mRBO);

    if (mTexture)
        glDeleteTextures(1, &mTexture);

    mCreated = false;
}

void Canavar::Engine::OpenGLFramebuffer::bind()
{
    if (mCreated)
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void Canavar::Engine::OpenGLFramebuffer::release()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
