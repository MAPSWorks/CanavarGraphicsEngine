#include "Texture.h"

#include <QDebug>

Texture::Texture(Type type, const QString &path, QObject *parent)
    : QObject(parent)
    , mType(type)
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    QImage image(path);
    image = image.convertToFormat(QImage::Format_RGBA8888);

    qInfo() << "Texture" << path << "is loaded and created. ID is" << mId << "Type is:" << int(mType);

    glBindTexture(GL_TEXTURE_2D, mId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(int width, int height, QObject *parent)
    : QObject(parent)
    , mType(Type::Texture2D)
    , mWidth(width)
    , mHeight(height)
    , mDepth(0)
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
}

Texture::Texture(int width, int height, int depth, QObject *parent)
    : QObject(parent)
    , mType(Type::Texture3D)
    , mWidth(width)
    , mHeight(height)
    , mDepth(depth)
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mId);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, mWidth, mHeight, mDepth, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_3D);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mId);
}

unsigned int Texture::id() const
{
    return mId;
}

Texture::Type Texture::type() const
{
    return mType;
}

const QString &Texture::path() const
{
    return mPath;
}

int Texture::width() const
{
    return mWidth;
}

int Texture::height() const
{
    return mHeight;
}

int Texture::depth() const
{
    return mDepth;
}
