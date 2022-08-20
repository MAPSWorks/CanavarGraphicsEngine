#include "NozzleEffect.h"

NozzleEffect::NozzleEffect(QObject *parent)
    : Node(parent)
{
    mName = "Nozzle Effect";
    mNodeType = Node::NodeType::NozzleEffect;
    mRenderable = false;
}

NozzleEffect::~NozzleEffect() {}

void NozzleEffect::render()
{
    mModelData->render(GL_TRIANGLES);
}

ModelData *NozzleEffect::modelData() const
{
    return mModelData;
}

void NozzleEffect::setModelData(ModelData *newModelData)
{
    mModelData = newModelData;
}
