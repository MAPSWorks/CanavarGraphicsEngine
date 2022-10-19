#include "Light.h"

Canavar::Engine::Light::Light(QObject *parent)
    : Node{parent}
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
{
    mRenderable = false;
}

Canavar::Engine::Light::~Light()
{
    // TODO
}

const QVector4D &Canavar::Engine::Light::color() const
{
    return mColor;
}

void Canavar::Engine::Light::setColor(const QVector4D &newColor)
{
    mColor = newColor;
}

float Canavar::Engine::Light::ambient() const
{
    return mAmbient;
}

void Canavar::Engine::Light::setAmbient(float newAmbient)
{
    mAmbient = newAmbient;
}

float Canavar::Engine::Light::diffuse() const
{
    return mDiffuse;
}

void Canavar::Engine::Light::setDiffuse(float newDiffuse)
{
    mDiffuse = newDiffuse;
}

float Canavar::Engine::Light::specular() const
{
    return mSpecular;
}

void Canavar::Engine::Light::setSpecular(float newSpecular)
{
    mSpecular = newSpecular;
}
