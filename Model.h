#ifndef MODEL_H
#define MODEL_H

#include "Material.h"
#include "Node.h"

#include <QOpenGLTexture>

class Model : public Node
{
public:
    explicit Model(QObject *parent = nullptr);
    virtual ~Model();

    enum Type { //
        Capsule,
        Cone,
        Cube,
        Cylinder,
        Dome,
        Plane,
        Pyramid,
        Sphere,
        Suzanne,
        Tetrahedron,
        Torus,
        TorusKnot,
    };

    Type type() const;
    void setType(Type newType);

    Material &material();
    void setMaterial(const Material &newMaterial);

    static const QVector<Model::Type> ALL_MODEL_TYPES;

private:
    Type mType;
    Material mMaterial;
};

#endif // MODEL_H
