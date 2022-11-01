#ifndef MODELDATANODE_H
#define MODELDATANODE_H

#include "Node.h"

#include <QOpenGLFunctions>

namespace Canavar {
namespace Engine {

class ModelData;

class ModelDataNode : public Node
{
public:
    explicit ModelDataNode(ModelData *data, QObject *parent = nullptr);

    void addMeshIndex(int index);

    void render(RenderModes modes, Model *model, GLenum primitive = GL_TRIANGLES);

private:
    ModelData *mModelData;
    QVector<int> mMeshIndices;
};

} // namespace Engine
} // namespace Canavar
#endif // MODELDATANODE_H
