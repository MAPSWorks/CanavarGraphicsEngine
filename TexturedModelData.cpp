#include "TexturedModelData.h"

TexturedModelData::TexturedModelData(QString path, QObject *parent)
    : QObject(parent)
    , mPath(path)
{}

void TexturedModelData::addMesh(Mesh *mesh)
{
    mesh->setParent(this);
    mMeshes << mesh;
}

void TexturedModelData::addTextureMaterial(TextureMaterial *material)
{
    material->setParent(this);
    mMaterials << material;
}

const QVector<Mesh *> &TexturedModelData::meshes() const
{
    return mMeshes;
}

const QVector<TextureMaterial *> &TexturedModelData::materials() const
{
    return mMaterials;
}

const QString &TexturedModelData::name() const
{
    return mName;
}

void TexturedModelData::setName(const QString &newName)
{
    mName = newName;
}

TexturedModelDataNode *TexturedModelData::rootNode() const
{
    return mRootNode;
}

void TexturedModelData::setRootNode(TexturedModelDataNode *newRootNode)
{
    mRootNode = newRootNode;
}

const QString &TexturedModelData::path() const
{
    return mPath;
}
