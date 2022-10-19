#ifndef MODELDATANODE_H
#define MODELDATANODE_H

#include "Common.h"
#include "Node.h"

namespace Canavar {
namespace Engine {

class ModelData;

class ModelDataNode : public Node
{
public:
    explicit ModelDataNode(ModelData *data, QObject *parent = nullptr);
    virtual ~ModelDataNode();

    void addMeshIndex(int index);

    void render(const RenderParameters &parameters);

private:
    ModelData *mModelData;
    QVector<int> mMeshIndices;
};

} // namespace Engine
} // namespace Canavar
#endif // MODELDATANODE_H
