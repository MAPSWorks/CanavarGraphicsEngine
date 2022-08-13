#include "Texture.h"

Texture::Texture(Type type, const QString &path, QObject *parent)
    : QObject(parent)
    , mType(type)
    , mPath(path)
{}

Texture::~Texture()
{
    // TODO
}

void Texture::create()
{
    initializeOpenGLFunctions();

    unsigned int id;
    glGenTextures(1, &id);

    QImage image(mPath);
    QImage formattedImage = image.convertToFormat(QImage::Format_RGB888);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, formattedImage.width(), formattedImage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, formattedImage.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mId = id;
}

Texture::Type Texture::type() const
{
    return mType;
}

const QString &Texture::path() const
{
    return mPath;
}

unsigned int Texture::id() const
{
    return mId;
}
