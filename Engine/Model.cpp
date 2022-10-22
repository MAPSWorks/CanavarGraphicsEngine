#include "Model.h"

Canavar::Engine::Model::Model(const QString &name, Node *parent)
    : Node(parent)
    , mModelName(name)
    , mColor(1, 1, 1, 1)
    , mAmbient(0.75f)
    , mDiffuse(0.75)
    , mSpecular(0.25)
    , mShininess(32.0f)
{
    mName = name;
    mType = Node::NodeType::Model;
    mRenderable = true;
}

QMatrix4x4 Canavar::Engine::Model::getMeshTransformation(const QString &meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}
