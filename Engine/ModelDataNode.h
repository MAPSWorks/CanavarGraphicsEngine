#ifndef MODELDATANODE_H
#define MODELDATANODE_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class ModelData;

class ModelDataNode : public Node
{
public:
    explicit ModelDataNode(ModelData *data, QObject *parent = nullptr);

    void addMeshIndex(int index);

    void render(RenderPasses renderPasses, Node *node);

private:
    ModelData *mModelData;
    QVector<int> mMeshIndices;
};

} // namespace Engine
} // namespace Canavar
#endif // MODELDATANODE_H
