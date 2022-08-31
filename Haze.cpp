#include "Haze.h"

Haze::Haze(QObject *parent)
    : QObject(parent)
{
    reset();
}

Haze *Haze::instance()
{
    static Haze instance;
    return &instance;
}

float Haze::gradient() const
{
    return mGradient;
}

void Haze::setGradient(float newGradient)
{
    mGradient = newGradient;
}

float Haze::density() const
{
    return mDensity;
}

void Haze::setDensity(float newDensity)
{
    mDensity = newDensity;
}

const QVector3D &Haze::color() const
{
    return mColor;
}

void Haze::setColor(const QVector3D &newColor)
{
    mColor = newColor;
}

bool Haze::enabled() const
{
    return mEnabled;
}

void Haze::setEnabled(bool newEnabled)
{
    mEnabled = newEnabled;
}

void Haze::drawGUI()
{
    if (!ImGui::CollapsingHeader("Haze"))
    {
        ImGui::SliderFloat("Density##Haze", &mDensity, 0.0f, 4.0f, "%.3f");
        ImGui::SliderFloat("Gradient##Haze", &mGradient, 0.0f, 4.0f, "%.3f");

        float color[3] = {mColor.x(), mColor.y(), mColor.z()};

        if (ImGui::ColorEdit3("Color##Haze", (float *) &color))
            mColor = QVector3D(color[0], color[1], color[2]);

        ImGui::Checkbox("Enabled##Haze", &mEnabled);

        if (ImGui::Button("Reset##Haze"))
            reset();
    }
}

void Haze::reset()
{
    mEnabled = true;
    mColor = QVector3D(0.33f, 0.38f, 0.47f);
    mDensity = 1.0f;
    mGradient = 1.5f;
}
