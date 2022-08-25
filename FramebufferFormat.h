#ifndef FRAMEBUFFERFORMAT_H
#define FRAMEBUFFERFORMAT_H

#include <QMap>
#include <QObject>

class FramebufferFormat
{
public:
    enum class TextureTarget : unsigned int { // OpenGL values
        Texture2D = 0x0DE1,
        Texture2DMultisample = 0x9100,
    };

    enum class TextureInternalFormat : unsigned int { // OpenGL values
        RGBA = 0x1908,
        RGBA8 = 0x8058,
        RGBA32F = 0x8814
    };

    FramebufferFormat();

    void addColorAttachment(unsigned int index, TextureTarget target, TextureInternalFormat format);

    const QMap<unsigned int, TextureInternalFormat> &textureInternalFormats() const;
    const QMap<unsigned int, TextureTarget> &textureTargets() const;
    const QVector<unsigned int> &textureIndices() const;

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

    int mWidth;
    int mHeight;
    int mSamples;
};

#endif // FRAMEBUFFERFORMAT_H
