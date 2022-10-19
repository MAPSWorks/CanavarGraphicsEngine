#include "Material.h"

Canavar::Engine::Material::Material(QObject *parent)
    : QObject(parent)
{}

Canavar::Engine::Material::~Material()
{
    // TODO
}

void Canavar::Engine::Material::insert(TextureType type, QOpenGLTexture *texture)
{
    mTextures.insert(type, texture);
}

QOpenGLTexture *Canavar::Engine::Material::get(TextureType type)
{
    return mTextures.value(type, nullptr);
}
