#include "Model.h"

Model::Model(const QString &modelName, QObject *parent)
    : Node(parent)
    , mModelName(modelName)
{
    mName = mModelName;
    mNodeType = Node::NodeType::Model;

    mMaterial.color = QVector4D(1, 1, 1, 1);
    mMaterial.ambient = 0.5f;
    mMaterial.diffuse = 0.75f;
    mMaterial.specular = 0.25f;
    mMaterial.shininess = 32.0f;
}

Model::~Model()
{

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

QMatrix4x4 Model::getMeshTransformation(const QString &meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Model::setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}

const QMap<QString, QMatrix4x4> &Model::meshTransformations() const
{
    return mMeshTransformations;
}
