#ifndef MODEL_H
#define MODEL_H

#include "Node.h"

class Model : public Node
{
protected:
    friend class NodeManager;

    explicit Model(const QString &modelName, QObject *parent = nullptr);
    virtual ~Model();

public:
    struct Material {
        QVector4D color;
        float ambient;
        float diffuse;
        float specular;
        float shininess;
    };

    const QString &modelName() const;
    const QMap<QString, QMatrix4x4> &meshTransformations() const;

    const Material &material() const;
    void setMaterial(const Material &newMaterial);

    QMatrix4x4 getMeshTransformation(const QString &meshName);
    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

    virtual void drawGUI();

protected:
    Material mMaterial;
    QString mModelName;
    QMap<QString, QMatrix4x4> mMeshTransformations;
};

#endif // MODEL_H
