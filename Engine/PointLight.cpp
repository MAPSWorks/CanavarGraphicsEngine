#include "PointLight.h"

Canavar::Engine::PointLight::PointLight()
    : Light()
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.001f)
{
    mType = Canavar::Engine::Node::NodeType::PointLight;
}
