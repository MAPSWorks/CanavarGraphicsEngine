#include "ModelDataNode.h"

#include "ModelData.h"

Canavar::Engine::ModelDataNode::ModelDataNode(ModelData *data, QObject *parent)
    : Node(parent)
    , mModelData(data)
{}

void Canavar::Engine::ModelDataNode::addMeshIndex(int index)
{
    mMeshIndices << index;
}

void Canavar::Engine::ModelDataNode::render(RenderPass renderPass, Node *node)
{
    auto meshes = mModelData->meshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->render(renderPass, node);

    for (int i = 0; i < mChildren.size(); i++)
        if (auto child = dynamic_cast<ModelDataNode *>(mChildren[i]))
            child->render(renderPass, node);
}
