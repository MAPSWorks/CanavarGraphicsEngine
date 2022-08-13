#include "TexturedModelData.h"

TexturedModelData::TexturedModelData(const QString &name, QObject *parent)
    : QObject(parent)
    , mName(name)
{}

void TexturedModelData::addMesh(Mesh *mesh)
{
    mesh->setParent(this);
    mMeshes << mesh;
}

void TexturedModelData::addMaterial(TexturedMaterial *material)
{
    material->setParent(this);
    mMaterials << material;
}

const QVector<Mesh *> &TexturedModelData::meshes() const
{
    return mMeshes;
}

const QString &TexturedModelData::name() const
{
    return mName;
}

void TexturedModelData::render()
{
    mRootNode->render();
}

const QVector<TexturedMaterial *> &TexturedModelData::materials() const
{
    return mMaterials;
}

TexturedModelDataNode *TexturedModelData::rootNode() const
{
    return mRootNode;
}

void TexturedModelData::setRootNode(TexturedModelDataNode *newRootNode)
{
    mRootNode = newRootNode;
}
