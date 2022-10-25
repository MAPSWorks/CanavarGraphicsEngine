#ifndef MODEL_H
#define MODEL_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class Model : public Node
{
protected:
    friend class Canavar::Engine::NodeManager;
    explicit Model(const QString &modelName, Node *parent = nullptr);

public:
    QMatrix4x4 getMeshTransformation(const QString &meshName);
    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

    QVector4D getMeshOverlayColor(const QString &meshName);
    void setMeshOverlayColor(const QString &meshName, const QVector4D &color);

    float getMeshOverlayColorFactor(const QString &meshName);
    void setMeshOverlayColorFactor(const QString &meshName, float factor);

    const QString &getModelName() const { return mModelName; }

protected:
    QMap<QString, QMatrix4x4> mMeshTransformations;
    QMap<QString, QVector4D> mMeshOverlayColors;
    QMap<QString, float> mMeshOverlayColorFactors;
    QString mModelName;

    DECLARE_MEMBER(QVector4D, Color)
    DECLARE_MEMBER(QVector4D, OverlayColor)
    DECLARE_MEMBER(float, OverlayColorFactor)
    DECLARE_MEMBER(float, Ambient)
    DECLARE_MEMBER(float, Diffuse)
    DECLARE_MEMBER(float, Specular)
    DECLARE_MEMBER(float, Shininess)
};

} // namespace Engine
} // namespace Canavar

#endif // MODEL_H
