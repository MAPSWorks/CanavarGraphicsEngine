#include "TexturedMaterial.h"

TexturedMaterial::TexturedMaterial(QObject *parent)
    : QObject(parent)
{}

void TexturedMaterial::addTexture(Texture *texture)
{
    texture->setParent(this);
    mTextures << texture;
}

const QVector<Texture *> &TexturedMaterial::textures() const
{
    return mTextures;
}
