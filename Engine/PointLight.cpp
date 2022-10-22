#include "PointLight.h"

Canavar::Engine::PointLight::PointLight(QObject *parent)
    : Light{parent}
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.032f)
{
    mType = Canavar::Engine::Node::NodeType::PointLight;
    mRenderable = false;
    mVisible = true;
}
