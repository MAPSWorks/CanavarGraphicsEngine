#include "TexturedModel.h"

TexturedModel::TexturedModel(const QString &modelName, QObject *parent)
    : Node(parent)
    , mShininess(32.0f)
    , mModelName(modelName)
{
    mNodeType = Node::NodeType::TexturedModel;
}

float TexturedModel::shininess() const
{
    return mShininess;
}

void TexturedModel::setShininess(float newShininess)
{
    mShininess = newShininess;
}

const QString &TexturedModel::modelName() const
{
    return mModelName;
}
