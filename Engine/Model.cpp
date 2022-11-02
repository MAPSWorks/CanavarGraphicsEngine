#include "Model.h"

Canavar::Engine::Model::Model(const QString &modelName)
    : Node()
    , mModelName(modelName)
    , mColor(1, 1, 1, 1)
    , mOverlayColor(1, 1, 1, 1)
    , mOverlayColorFactor(0.0f)
    , mAmbient(0.8f)
    , mDiffuse(0.75)
    , mSpecular(0.25)
    , mShininess(32.0f)
{
    mName = modelName;
    mType = Node::NodeType::Model;
}

QMatrix4x4 Canavar::Engine::Model::getMeshTransformation(const QString &meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}

QVector4D Canavar::Engine::Model::getMeshOverlayColor(const QString &meshName)
{
    return mMeshOverlayColors.value(meshName, QVector4D(0, 0, 0, 0));
}

void Canavar::Engine::Model::setMeshOverlayColor(const QString &meshName, const QVector4D &color)
{
    mMeshOverlayColors.insert(meshName, color);
}

float Canavar::Engine::Model::getMeshOverlayColorFactor(const QString &meshName)
{
    return mMeshOverlayColorFactors.value(meshName, 0);
}

void Canavar::Engine::Model::setMeshOverlayColorFactor(const QString &meshName, float factor)
{
    mMeshOverlayColorFactors.insert(meshName, factor);
}
