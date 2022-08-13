#include "TexturedModelDataNode.h"
#include "TexturedModelData.h"

TexturedModelDataNode::TexturedModelDataNode(TexturedModelData *data, QObject *parent)
    : QObject(parent)
    , mData(data)
{}

void TexturedModelDataNode::addChild(TexturedModelDataNode *child)
{
    child->setParent(this);
    mChildren << child;
}

void TexturedModelDataNode::addMeshIndex(int index)
{
    mMeshIndices << index;
}

const QVector<TexturedModelDataNode *> &TexturedModelDataNode::children() const
{
    return mChildren;
}

const QString &TexturedModelDataNode::name() const
{
    return mName;
}

void TexturedModelDataNode::setName(const QString &newName)
{
    setObjectName(newName);
    mName = newName;
}

const QMatrix4x4 &TexturedModelDataNode::initialTransformation() const
{
    return mInitialTransformation;
}

void TexturedModelDataNode::setInitialTransformation(const QMatrix4x4 &newInitialTransformation)
{
    mInitialTransformation = newInitialTransformation;
}

void TexturedModelDataNode::render()
{
    auto meshes = mData->meshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->render();

    for (int i = 0; i < mChildren.size(); i++)
        mChildren[i]->render();
}
