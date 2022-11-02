#include "Light.h"

Canavar::Engine::Light::Light(const QString &uuid)
    : Node(uuid)
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
{}
