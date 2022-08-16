#include "TexturedModelData.h"

TexturedModelData::TexturedModelData(const QString &modelName, QObject *parent)
    : QObject(parent)
    , mModelName(modelName)
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

const QString &TexturedModelData::modelName() const
{
    return mModelName;
}

void TexturedModelData::render()
{
    mRootNode->render();
}

void TexturedModelData::renderWireframe()
{
    mRootNode->renderWireframe();
}

void TexturedModelData::renderNormals()
{
    mRootNode->renderNormals();
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
