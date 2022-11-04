#ifndef MODEL_H
#define MODEL_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class Model : public Node
{
protected:
    friend class Canavar::Engine::NodeManager;
    Model(const QString &modelName);

public:
    QMatrix4x4 getMeshTransformation(const QString &meshName);
    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

    QVector4D getMeshOverlayColor(const QString &meshName);
    void setMeshOverlayColor(const QString &meshName, const QVector4D &color);

    float getMeshOverlayColorFactor(const QString &meshName);
    void setMeshOverlayColorFactor(const QString &meshName, float factor);

    const QString &getModelName() const { return mModelName; }

    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

protected:
    QMap<QString, QMatrix4x4> mMeshTransformations;
    QMap<QString, QVector4D> mMeshOverlayColors;
    QMap<QString, float> mMeshOverlayColorFactors;
    QString mModelName;

    DEFINE_MEMBER(QVector4D, Color)
    DEFINE_MEMBER(QVector4D, OverlayColor)
    DEFINE_MEMBER(float, OverlayColorFactor)
    DEFINE_MEMBER(float, Ambient)
    DEFINE_MEMBER(float, Diffuse)
    DEFINE_MEMBER(float, Specular)
    DEFINE_MEMBER(float, Shininess)
};

} // namespace Engine
} // namespace Canavar

#endif // MODEL_H
