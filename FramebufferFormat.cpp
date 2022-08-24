#include "FramebufferFormat.h"

FramebufferFormat::FramebufferFormat()
    : mAttachment(Attachment::NoAttachment)
    , mWidth(0)
    , mHeight(0)
    , mSamples(0)
{}

void FramebufferFormat::addColorAttachment(unsigned int index, TextureTarget target, TextureInternalFormat format)
{
    mTextureIndices << index;
    mTextureTargets.insert(index, target);
    mTextureInternalFormats.insert(index, format);
}

const QMap<unsigned int, FramebufferFormat::TextureInternalFormat> &FramebufferFormat::textureInternalFormats() const
{
    return mTextureInternalFormats;
}

const QMap<unsigned int, FramebufferFormat::TextureTarget> &FramebufferFormat::textureTargets() const
{
    return mTextureTargets;
}

int FramebufferFormat::width() const
{
    return mWidth;
}

void FramebufferFormat::setWidth(int newWidth)
{
    mWidth = newWidth;
}

int FramebufferFormat::height() const
{
    return mHeight;
}

void FramebufferFormat::setHeight(int newHeight)
{
    mHeight = newHeight;
}

int FramebufferFormat::samples() const
{
    return mSamples;
}

void FramebufferFormat::setSamples(int newSamples)
{
    mSamples = newSamples;
}

const QVector<unsigned int> &FramebufferFormat::textureIndices() const
{
    return mTextureIndices;
}

FramebufferFormat::Attachment FramebufferFormat::attachment() const
{
    return mAttachment;
}

void FramebufferFormat::setAttachment(Attachment newAttachment)
{
    mAttachment = newAttachment;
}
