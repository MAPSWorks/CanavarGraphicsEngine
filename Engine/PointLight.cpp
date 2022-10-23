#include "PointLight.h"

Canavar::Engine::PointLight::PointLight(QObject *parent)
    : Light(parent)
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.001f)
{
    mType = Canavar::Engine::Node::NodeType::PointLight;
    mRenderable = true;
    mVisible = true;
}
