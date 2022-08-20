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

void ModelDataNode::render(Model *model)
{
    auto meshes = mData->meshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->render(model);

    for (int i = 0; i < mChildren.size(); i++)
        mChildren[i]->render(model);
}

QMatrix4x4 ModelDataNode::getWorldTransformation() const
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
    {
        QMatrix4x4 parentWorldTransformation = parent->worldTransformation();
        float x = parent->scale().x();
        float y = parent->scale().y();
        float z = parent->scale().z();

        parentWorldTransformation.scale(1 / x, 1 / y, 1 / z);

        return parentWorldTransformation * transformation();
    } else
    {
        return transformation();
    }
}

ModelDataNode *ModelDataNode::getChildNode(const QString &nodeName)
{
    return getChildNode(nodeName, this);
}

ModelDataNode *ModelDataNode::getChildNode(const QString &nodeName, ModelDataNode *node)
{
    if (node->name() == nodeName)
        return node;

    auto children = node->children();
    for (const auto &childNode : children)
    {
        ModelDataNode *result = getChildNode(nodeName, childNode);
        if (result)
            return result;
    }

    return nullptr;
}

const QMatrix4x4 &ModelDataNode::transformation() const
{
    return mTransformation;
}

void ModelDataNode::setTransformation(const QMatrix4x4 &newTransformation)
{
    mTransformation = newTransformation;
}
