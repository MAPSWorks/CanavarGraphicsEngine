#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(QObject *parent)
    : Light(parent)
    , mDirection(1, -1, 1)

{
    mAmbient = 0.8f;
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
