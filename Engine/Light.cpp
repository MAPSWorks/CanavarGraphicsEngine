#include "Light.h"

Canavar::Engine::Light::Light(QObject *parent)
    : Node{parent}
    , mModelName("SphereDense")
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
    , mModelColor(1.0f, 1.0f, 1.0f, 0.1f)
    , mModelAmbient(1.0f)
    , mModelDiffuse(1.0f)
    , mModelSpecular(1.0f)
    , mModelShininess(32.0f)
{
    mRenderable = false;
}
