#include "Model.h"

Canavar::Engine::Model::Model(const QString &name, Node *parent)
    : Node(parent)
    , mModelName(name)
    , mColor(1, 1, 1, 1)
    , mAmbient(0.5f)
    , mDiffuse(0.75)
    , mSpecular(0.25)
    , mShininess(32.0f)
{
    mName = name;
    mType = Node::NodeType::Model;
    mRenderable = true;
}

Canavar::Engine::Model::~Model()
{
    // TODO
}

const QVector4D &Canavar::Engine::Model::color() const
{
    return mColor;
}

void Canavar::Engine::Model::setColor(const QVector4D &newColor)
{
    mColor = newColor;
}

float Canavar::Engine::Model::ambient() const
{
    return mAmbient;
}

void Canavar::Engine::Model::setAmbient(float newAmbient)
{
    mAmbient = newAmbient;
}

float Canavar::Engine::Model::diffuse() const
{
    return mDiffuse;
}

void Canavar::Engine::Model::setDiffuse(float newDiffuse)
{
    mDiffuse = newDiffuse;
}

float Canavar::Engine::Model::specular() const
{
    return mSpecular;
}

void Canavar::Engine::Model::setSpecular(float newSpecular)
{
    mSpecular = newSpecular;
}

float Canavar::Engine::Model::shininess() const
{
    return mShininess;
}

void Canavar::Engine::Model::setShininess(float newShininess)
{
    mShininess = newShininess;
}

QMatrix4x4 Canavar::Engine::Model::getMeshTransformation(const QString &meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}

const QString &Canavar::Engine::Model::modelName() const
{
    return mModelName;
}

void Canavar::Engine::Model::setModelName(const QString &newModelName)
{
    mModelName = newModelName;
}
