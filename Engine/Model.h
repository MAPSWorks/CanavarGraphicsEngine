#ifndef MODEL_H
#define MODEL_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class Model : public Node
{
protected:
    friend class Canavar::Engine::NodeManager;
    explicit Model(const QString &name, Node *parent = nullptr);

public:
    QMatrix4x4 getMeshTransformation(const QString &meshName);
    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

protected:
    QMap<QString, QMatrix4x4> mMeshTransformations;

    DECLARE_MEMBER(QString, ModelName)
    DECLARE_MEMBER(QVector4D, Color)
    DECLARE_MEMBER(float, Ambient)
    DECLARE_MEMBER(float, Diffuse)
    DECLARE_MEMBER(float, Specular)
    DECLARE_MEMBER(float, Shininess)
};

} // namespace Engine
} // namespace Canavar

#endif // MODEL_H
