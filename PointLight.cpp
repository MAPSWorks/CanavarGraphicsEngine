#include "PointLight.h"

PointLight::PointLight(QObject *parent)
    : Light(parent)
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.032f)
{
    mNodeType = Node::NodeType::PointLight;
}

float PointLight::constant() const
{
    return mConstant;
}

void PointLight::setConstant(float newConstant)
{
    mConstant = newConstant;
}

float PointLight::linear() const
{
    return mLinear;
}

void PointLight::setLinear(float newLinear)
{
    mLinear = newLinear;
}

float PointLight::quadratic() const
{
    return mQuadratic;
}

void PointLight::setQuadratic(float newQuadratic)
{
    mQuadratic = newQuadratic;
}
