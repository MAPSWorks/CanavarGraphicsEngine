#include "SpotLight.h"

SpotLight::SpotLight(QObject *parent)
    : PointLight(parent)
    , mDirection(0, -1, 0)
    , mCutOffAngle(0.25f)
    , mOuterCutOffAngle(1.0f)
{
    mNodeType = Node::NodeType::SpotLight;
}

SpotLight::~SpotLight() {}

const QVector3D &SpotLight::direction() const
{
    return mDirection;
}

void SpotLight::setDirection(const QVector3D &newDirection)
{
    mDirection = newDirection;
}

float SpotLight::cutOffAngle() const
{
    return mCutOffAngle;
}

void SpotLight::setCutOffAngle(float newCutOffAngle)
{
    mCutOffAngle = newCutOffAngle;
}

float SpotLight::outerCutOffAngle() const
{
    return mOuterCutOffAngle;
}

void SpotLight::setOuterCutOffAngle(float newOuterCutOffAngle)
{
    mOuterCutOffAngle = newOuterCutOffAngle;
}

void SpotLight::drawGui()
{
    PointLight::drawGui();

    if (!ImGui::CollapsingHeader("Direction##SpotLight"))
    {
        float x = mDirection.x();
        float y = mDirection.y();
        float z = mDirection.z();

        if (ImGui::DragFloat("x##SpotLightDirection", &x, 0.01f, -1.0f, 1.0f, "%.3f"))
            mDirection = QVector3D(x, y, z).normalized();
        if (ImGui::DragFloat("y##SpotLightDirection", &y, 0.01f, -1.0f, 1.0f, "%.3f"))
            mDirection = QVector3D(x, y, z).normalized();
        if (ImGui::DragFloat("z##SpotLightDirection", &z, 0.01f, -1.0f, 1.0f, "%.3f"))
            mDirection = QVector3D(x, y, z).normalized();
    }

    if (!ImGui::CollapsingHeader("Angles##SpotLight"))
    {
        ImGui::SliderFloat("Cutt Off Angle##SpotLightDirection", &mCutOffAngle, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Outer Cutt Off Angle##SpotLightDirection", &mOuterCutOffAngle, 0.0f, 1.0f, "%.3f");
    }
}
