#include "Framebuffer.h"

#include <QDebug>

Framebuffer::Framebuffer(FramebufferFormat format, QObject *parent)
    : QObject(parent)
    , mFormat(format)
    , mFramebuffer(0)
    , mRenderObject(0)
    , mDepth(0)

{
    initializeOpenGLFunctions();
    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    unsigned int attachment = (unsigned int) mFormat.attachment();
    auto textureIndices = mFormat.textureIndices();
    auto textureTargets = mFormat.textureTargets();
    auto textureInternalFormats = mFormat.textureInternalFormats();
    auto samples = mFormat.samples();

    if (attachment & 0x02) // Depth and Stencil Attachment
    {
        if (samples == 0)
        {
            glGenRenderbuffers(1, &mRenderObject);
            glBindRenderbuffer(GL_RENDERBUFFER, mRenderObject);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mFormat.width(), mFormat.height());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderObject);

        } else
        {
            glGenRenderbuffers(1, &mRenderObject);
            glBindRenderbuffer(GL_RENDERBUFFER, mRenderObject);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, mFormat.width(), mFormat.height());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderObject);
        }
    }

    if (attachment & 0x01) // Color Attachment
    {
        for (auto index : textureIndices)
        {
            unsigned int id;
            unsigned int target = (unsigned int) textureTargets.value(index, FramebufferFormat::TextureTarget::Texture2D);
            unsigned int format = (unsigned int) textureInternalFormats.value(index, FramebufferFormat::TextureInternalFormat::RGBA8);

            if (target == GL_TEXTURE_2D)
            {
                glGenTextures(1, &id);
                glBindTexture(GL_TEXTURE_2D, id);
                glTexImage2D(GL_TEXTURE_2D, 0, format, mFormat.width(), mFormat.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);

            } else if (target == GL_TEXTURE_2D_MULTISAMPLE)
            {
                glGenTextures(1, &id);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
                glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, mFormat.width(), mFormat.height(), GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, id, 0);
            }

            mTextures.insert(index, id);
        }
    }

    else if (attachment & 0x04) // Depth Texture Attachment
    {
        if (samples == 0)
        {
            glGenTextures(1, &mDepth);
            glBindTexture(GL_TEXTURE_2D, mDepth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_BUFFER_BIT, mFormat.width(), mFormat.height(), 0, GL_DEPTH_BUFFER_BIT, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepth, 0);
        } else
        {
            glGenTextures(1, &mDepth);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepth);
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH_BUFFER_BIT, mFormat.width(), mFormat.height(), GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_MODE, GL_NONE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, mDepth, 1);
        }
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qCritical() << "Framebuffer is could not be created.";
    }
}

Framebuffer::~Framebuffer()
{
    if (mFramebuffer)
        glDeleteFramebuffers(1, &mFramebuffer);

    if (mRenderObject)
        glDeleteBuffers(1, &mRenderObject);

    if (mDepth)
        glDeleteTextures(1, &mDepth);

    auto textureIndices = mTextures.keys();

    for (auto index : qAsConst(textureIndices))
    {
        auto texture = mTextures.value(index, 0);
        if (texture)
        {
            glDeleteTextures(1, &texture);
        }

        mTextures.insert(index, texture);
    }
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

unsigned int Framebuffer::texture(int index)
{
    return mTextures.value(index);
}

unsigned int Framebuffer::depth() const
{
    return mDepth;
}

const FramebufferFormat &Framebuffer::format() const
{
    return mFormat;
}
