#ifndef MODEL_H
#define MODEL_H

#include "Node.h"

class Model : public Node
{
public:
    struct Material {
        QVector4D color;
        float ambient;
        float diffuse;
        float specular;
        float shininess;
    };

    explicit Model(const QString &modelName, QObject *parent = nullptr);

    const QString &modelName() const;
    const QMap<QString, QMatrix4x4> &meshTransformations() const;

    const Material &material() const;
    void setMaterial(const Material &newMaterial);

    void setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation);

private:
    Material mMaterial;
    QString mModelName;
    QMap<QString, QMatrix4x4> mMeshTransformations;
};

#endif // MODEL_H
