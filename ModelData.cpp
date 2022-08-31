#include "ModelData.h"

ModelData::ModelData(const QString &modelName, QObject *parent)
    : QObject(parent)
    , mModelName(modelName)
{}

void ModelData::addMesh(Mesh *mesh)
{
    mesh->setParent(this);
    mMeshes << mesh;
}

void ModelData::addMaterial(TextureMaterial *material)
{
    material->setParent(this);
    mMaterials << material;
}

const QVector<Mesh *> &ModelData::meshes() const
{
    return mMeshes;
}

const QString &ModelData::modelName() const
{
    return mModelName;
}

void ModelData::render(Model *model, const RenderSettings &settings)
{
    mRootNode->render(model, settings);
}

Mesh *ModelData::getMesh(const QString &meshName)
{
    for (auto const &mesh : qAsConst(mMeshes))
        if (mesh->name() == meshName)
            return mesh;

    return nullptr;
}

const QVector<TextureMaterial *> &ModelData::materials() const
{
    return mMaterials;
}

ModelDataNode *ModelData::rootNode() const
{
    return mRootNode;
}

void ModelData::setRootNode(ModelDataNode *newRootNode)
{
    mRootNode = newRootNode;
}
