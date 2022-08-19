#include "TextureMaterial.h"

TextureMaterial::TextureMaterial(QObject *parent)
    : QObject(parent)
{}

void TextureMaterial::addTexture(Texture *texture)
{
    texture->setParent(this);
    mTextures << texture;
}

const QVector<Texture *> &TextureMaterial::textures() const
{
    return mTextures;
}
