#include "ModelDataNode.h"
#include "ModelData.h"

ModelDataNode::ModelDataNode(ModelData *data, QObject *parent)
    : QObject(parent)
    , mData(data)
{}

void ModelDataNode::addChild(ModelDataNode *child)
{
    child->setParent(this);
    mChildren << child;
}

void ModelDataNode::addMeshIndex(int index)
{
    mMeshIndices << index;
}

const QVector<ModelDataNode *> &ModelDataNode::children() const
{
    return mChildren;
}

const QString &ModelDataNode::name() const
{
    return mName;
}

void ModelDataNode::setName(const QString &newName)
{
    setObjectName(newName);
    mName = newName;
}

const QMatrix4x4 &ModelDataNode::initialTransformation() const
{
    return mInitialTransformation;
}

void ModelDataNode::setInitialTransformation(const QMatrix4x4 &newInitialTransformation)
{
    mInitialTransformation = newInitialTransformation;
}

void ModelDataNode::render(Model *model)
{
    auto meshes = mData->meshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->render(model);

    for (int i = 0; i < mChildren.size(); i++)
        mChildren[i]->render(model);
}
