#ifndef FRAMEBUFFERFORMAT_H
#define FRAMEBUFFERFORMAT_H

#include <QMap>
#include <QObject>

class FramebufferFormat
{
public:
    enum class TextureTarget : unsigned int { // OpenGL values
        TEXTURE_2D = 0x0DE1,
        TEXTURE_2D_MULTISAMPLE = 0x9100,
    };

    enum class TextureInternalFormat : unsigned int { // OpenGL values
        RGBA = 0x1908,
        RGBA8 = 0x8058,
        RGBA32F = 0x8814,
        RGBA32UI = 0x8D70,
        RGB32UI = 0x8D71,
    };

    enum class TexturePixelFormat : unsigned int { // OpenGL values
        RGBA = 0x1908,
        RGB = 0x1907,
        RGB_INTEGER = 0x8D98,
        RGBA_INTEGER = 0x8D99
    };

    enum class TextureDataType : unsigned int { // OpenGL values
        UNSIGNED_BYTE = 0x1401,
        UNSIGNED_INT = 0x1405,
    };

    FramebufferFormat();

    void addColorAttachment(unsigned int index, TextureTarget target, TextureInternalFormat internalFormat, TexturePixelFormat pixelFormat, TextureDataType dataType);

    const QVector<unsigned int> &textureIndices() const;
    const QMap<unsigned int, TextureTarget> &textureTargets() const;
    const QMap<unsigned int, TextureInternalFormat> &textureInternalFormats() const;
    const QMap<unsigned int, TextureDataType> &textureDataTypes() const;
    const QMap<unsigned int, TexturePixelFormat> &texturePixelFormats() const;

    int width() const;
    void setWidth(int newWidth);

    int height() const;
    void setHeight(int newHeight);

    int samples() const;
    void setSamples(int newSamples);

private:
    QVector<unsigned int> mTextureIndices;
    QMap<unsigned int, TextureTarget> mTextureTargets;
    QMap<unsigned int, TextureInternalFormat> mTextureInternalFormats;
    QMap<unsigned int, TexturePixelFormat> mTexturePixelFormats;
    QMap<unsigned int, TextureDataType> mTextureDataTypes;

    int mWidth;
    int mHeight;
    int mSamples;
};

#endif // FRAMEBUFFERFORMAT_H
