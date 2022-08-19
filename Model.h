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

    const Material &material() const;
    void setMaterial(const Material &newMaterial);

private:
    Material mMaterial;
    QString mModelName;
};

#endif // MODEL_H
