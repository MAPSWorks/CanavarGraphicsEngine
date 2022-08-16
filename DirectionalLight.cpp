#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(QObject *parent)
    : Light(parent)
    , mDirection(0, -1, 0)

{
    mDiffuse = 0.5f;
    mNodeType = Node::NodeType::DirectionalLight;

    float inf = std::numeric_limits<float>::infinity();
    mPosition = QVector3D(inf, inf, inf);
}

const QVector3D &DirectionalLight::direction() const
{
    return mDirection;
}

void DirectionalLight::setDirection(const QVector3D &newDirection)
{
    mDirection = newDirection;
}
