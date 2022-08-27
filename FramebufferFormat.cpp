#include "FramebufferFormat.h"

FramebufferFormat::FramebufferFormat()
    : mWidth(0)
    , mHeight(0)
    , mSamples(0)
{}

void FramebufferFormat::addColorAttachment(unsigned int index, TextureTarget target, TextureInternalFormat internalFormat, TexturePixelFormat pixelFormat, TextureDataType dataType)
{
    mTextureIndices << index;
    mTextureTargets.insert(index, target);
    mTextureInternalFormats.insert(index, internalFormat);
    mTexturePixelFormats.insert(index, pixelFormat);
    mTextureDataTypes.insert(index, dataType);
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

const QMap<unsigned int, FramebufferFormat::TextureTarget> &FramebufferFormat::textureTargets() const
{
    return mTextureTargets;
}

const QMap<unsigned int, FramebufferFormat::TexturePixelFormat> &FramebufferFormat::texturePixelFormats() const
{
    return mTexturePixelFormats;
}

const QMap<unsigned int, FramebufferFormat::TextureInternalFormat> &FramebufferFormat::textureInternalFormats() const
{
    return mTextureInternalFormats;
}

const QMap<unsigned int, FramebufferFormat::TextureDataType> &FramebufferFormat::textureDataTypes() const
{
    return mTextureDataTypes;
}
