#include "PointLight.h"

Canavar::Engine::PointLight::PointLight()
    : Light()
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.001f)
{
    mType = Canavar::Engine::Node::NodeType::PointLight;
}

void Canavar::Engine::PointLight::toJson(QJsonObject &object)
{
    Light::toJson(object);

    object.insert("constant", mConstant);
    object.insert("linear", mLinear);
    object.insert("quadratic", mQuadratic);
}

void Canavar::Engine::PointLight::fromJson(const QJsonObject &object)
{
    Light::fromJson(object);

    mConstant = object["constant"].toDouble();
    mLinear = object["linear"].toDouble();
    mQuadratic = object["quadratic"].toDouble();
}
