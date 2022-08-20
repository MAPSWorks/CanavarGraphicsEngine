#include "Texture.h"

#include <QDebug>

Texture::Texture(Type type, const QString &path, QObject *parent)
    : QObject(parent)
    , mType(type)
    , mPath(path)
{}

Texture::~Texture() {}

void Texture::create()
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    QImage image(mPath);
    image = image.convertToFormat(QImage::Format_RGB888);

    qInfo() << "Texture" << mPath << "is loaded and created. ID is" << mId << "Type is:" << int(mType);

    glBindTexture(GL_TEXTURE_2D, mId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
