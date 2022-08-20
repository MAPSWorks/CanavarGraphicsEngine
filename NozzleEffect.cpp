#include "NozzleEffect.h"

NozzleEffect::NozzleEffect(const QString &modelName, QObject *parent)
    : Model(modelName)
    , mStrength(1.0f)
    , mBlurFactor(1)
{
    mNodeType = Node::NodeType::NozzleEffect;
    mMaterial.color = QVector4D(1, 1, 1, 1);
    mRenderable = false;
}

NozzleEffect::~NozzleEffect() {}

float NozzleEffect::strength() const
{
    return mStrength;
}

void NozzleEffect::setStrength(float newStrength)
{
    mStrength = newStrength;
}

int NozzleEffect::blurFactor() const
{
    return mBlurFactor;
}

void NozzleEffect::setBlurFactor(int newBlurFactor)
{
    mBlurFactor = newBlurFactor;
}
