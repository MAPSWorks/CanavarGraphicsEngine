#include "PointLight.h"

PointLight::PointLight(QObject *parent)
    : Light(parent)
    , mConstant(1.0f)
    , mLinear(0.09f)
    , mQuadratic(0.032f)
{
    mNodeType = Node::NodeType::PointLight;
}

PointLight::~PointLight() {}

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

void PointLight::drawGUI()
{
    Light::drawGUI();

    if (!ImGui::CollapsingHeader("Attenuation##PointLight"))
    {
        ImGui::SliderFloat("Constant##PointLight", &mConstant, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Linear##PointLight", &mLinear, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Quadratic##PointLight", &mQuadratic, 0.0f, 1.0f, "%.3f");
    }
}
