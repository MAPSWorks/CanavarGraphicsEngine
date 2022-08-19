#include "Model.h"

Model::Model(const QString &modelName, QObject *parent)
    : Node(parent)
    , mModelName(modelName)
{
    mName = mModelName;
    mNodeType = Node::NodeType::Model;

    mMaterial.color = QVector4D(1, 1, 1, 1);
    mMaterial.ambient = 0.25f;
    mMaterial.diffuse = 0.75f;
    mMaterial.specular = 0.25f;
    mMaterial.shininess = 32.0f;
}

const QString &Model::modelName() const
{
    return mModelName;
}

const Model::Material &Model::material() const
{
    return mMaterial;
}

void Model::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}
