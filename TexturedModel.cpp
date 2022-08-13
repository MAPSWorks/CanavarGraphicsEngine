#include "TexturedModel.h"

TexturedModel::TexturedModel(const QString &name, QObject *parent)
    : Node(parent)
    , mName(name)
    , mShininess(32.0f)
{}

const QString &TexturedModel::name() const
{
    return mName;
}

float TexturedModel::shininess() const
{
    return mShininess;
}

void TexturedModel::setShininess(float newShininess)
{
    mShininess = newShininess;
}
