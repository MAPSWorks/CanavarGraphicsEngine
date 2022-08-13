#include "TexturedModelDataNode.h"

TexturedModelDataNode::TexturedModelDataNode(QObject *parent)
    : QObject{parent}
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

const QVector<int> &TexturedModelDataNode::meshIndices() const
{
    return mMeshIndices;
}

void TexturedModelDataNode::setMeshIndices(const QVector<int> &newMeshIndices)
{
    mMeshIndices = newMeshIndices;
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
