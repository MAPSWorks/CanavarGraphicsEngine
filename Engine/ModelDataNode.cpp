#include "ModelDataNode.h"

#include "ModelData.h"

Canavar::Engine::ModelDataNode::ModelDataNode(ModelData *data)
    : Node()
    , mModelData(data)
{}

void Canavar::Engine::ModelDataNode::addMeshIndex(int index)
{
    mMeshIndices << index;
}

void Canavar::Engine::ModelDataNode::render(RenderModes modes, Model *model)
{
    auto meshes = mModelData->meshes();

    for (auto index : qAsConst(mMeshIndices))
        meshes[index]->render(modes, model);

    for (int i = 0; i < mChildren.size(); i++)
        if (auto child = dynamic_cast<ModelDataNode *>(mChildren[i]))
            child->render(modes, model);
}
