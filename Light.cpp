#include "Light.h"

Light::Light(QObject *parent)
    : Node{parent}
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
{
    mRenderable = false;
}

Light::~Light() {}

const QVector4D &Light::color() const
{
    return mColor;
}

void Light::setColor(const QVector4D &newColor)
{
    mColor = newColor;
}

float Light::ambient() const
{
    return mAmbient;
}

void Light::setAmbient(float newAmbient)
{
    mAmbient = newAmbient;
}

float Light::diffuse() const
{
    return mDiffuse;
}

void Light::setDiffuse(float newDiffuse)
{
    mDiffuse = newDiffuse;
}

float Light::specular() const
{
    return mSpecular;
}

void Light::setSpecular(float newSpecular)
{
    mSpecular = newSpecular;
}

void Light::drawGui()
{
    Node::drawGui();

    ImGui::SliderFloat("Ambient##Light", &mAmbient, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Diffuse##Light", &mDiffuse, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Specular##Light", &mSpecular, 0.0f, 1.0f, "%.3f");

    float colorf[4] = {mColor.x(), mColor.y(), mColor.z(), mColor.w()};

    if (ImGui::ColorEdit4("Color##Light", (float *) &colorf))
        mColor = QVector4D(colorf[0], colorf[1], colorf[2], colorf[3]);
}
