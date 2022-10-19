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
    virtual ~Model();

public:
    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    float ambient() const;
    void setAmbient(float newAmbient);

    float diffuse() const;
    void setDiffuse(float newDiffuse);

    float specular() const;
    void setSpecular(float newSpecular);

    float shininess() const;
    void setShininess(float newShininess);

    QMatrix4x4 getMeshTransformation(const QString &meshName);
    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

protected:
    QMap<QString, QMatrix4x4> mMeshTransformations;

    QVector4D mColor;
    float mAmbient;
    float mDiffuse;
    float mSpecular;
    float mShininess;
};

} // namespace Engine
} // namespace Canavar

#endif // MODEL_H
