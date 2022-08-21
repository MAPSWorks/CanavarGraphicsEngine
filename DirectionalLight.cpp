#include "DirectionalLight.h"

#include <QtMath>

DirectionalLight::DirectionalLight(QObject *parent)
    : Light(parent)
    , mDirection(0, -1, 0)

{
    mAmbient = 1.0f;
    mDiffuse = 0.75f;
    mNodeType = Node::NodeType::DirectionalLight;

    float inf = std::numeric_limits<float>::infinity();
    mPosition = QVector3D(inf, inf, inf);
}

const QVector3D &DirectionalLight::direction() const
{
    return mDirection;
}

void DirectionalLight::setDirection(const QVector3D &newDirection)
{
    mDirection = newDirection;
}

void DirectionalLight::drawGui()
{
    // Light
    if (!ImGui::CollapsingHeader("Sun"))
    {
        ImGui::Text("Direction:");
        float x = mDirection.x();
        float y = mDirection.y();
        float z = mDirection.z();
        float r = mDirection.length();
        float theta = qRadiansToDegrees(atan2(z, x));
        float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

        if (qFuzzyCompare(abs(phi), 90.0f))
            theta = 0.0f;

        bool updated = false;

        if (ImGui::SliderFloat("Theta##DirectionalLight", &theta, -180.0f, 180.0f, "%.1f"))
            updated = true;

        if (ImGui::SliderFloat("Phi##DirectionalLight", &phi, -90.0f, 90.0f, "%.1f"))
            updated = true;

        if (updated)
        {
            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            mDirection = QVector3D(x, y, z);
        }

        ImGui::Text("Shading Parameters:");
        ImGui::SliderFloat("Ambient##DirectionalLight", &mAmbient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##DirectionalLight", &mDiffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##DirectionalLight", &mSpecular, 0.0f, 1.0f, "%.3f");

        float colorf[4] = {mColor.x(), mColor.y(), mColor.z(), mColor.w()};

        if (ImGui::ColorEdit4("Color##DirectionalLight", (float *) &colorf))
            mColor = QVector4D(colorf[0], colorf[1], colorf[2], colorf[3]);
    }
}
