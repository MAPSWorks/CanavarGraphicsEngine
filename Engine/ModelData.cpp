#include "ModelData.h"

Canavar::Engine::ModelData::ModelData(const QString &name, QObject *parent)
    : QObject(parent)
    , mName(name)
    , mRootNode(nullptr)
{}

Canavar::Engine::ModelData::~ModelData()
{
    // TODO
}

void Canavar::Engine::ModelData::addMesh(Mesh *mesh)
{
    mMeshes << mesh;
}

void Canavar::Engine::ModelData::addMaterial(Material *material)
{
    mMaterials << material;
}

void Canavar::Engine::ModelData::setRootNode(ModelDataNode *newRootNode)
{
    mRootNode = newRootNode;
}

Canavar::Engine::Material *Canavar::Engine::ModelData::getMaterial(int index)
{
    return mMaterials[index];
}

Canavar::Engine::Mesh *Canavar::Engine::ModelData::getMeshByID(unsigned int id)
{
    for (const auto &mesh : mMeshes)
        if (mesh->getID() == id)
            return mesh;

    return nullptr;
}

const QString &Canavar::Engine::ModelData::name() const
{
    return mName;
}

const QVector<Canavar::Engine::Mesh *> &Canavar::Engine::ModelData::meshes() const
{
    return mMeshes;
}

void Canavar::Engine::ModelData::render(RenderModes modes, Model *model, GLenum primitive)
{
    mRootNode->render(modes, model, primitive);
}
