#include "Sun.h"

Canavar::Engine::Sun::Sun(QObject *parent)
    : Light{parent}
    , mDirection(0, -1, 0)
{
    mAmbient = 1.0f;
    mDiffuse = 0.75f;
    mType = Node::NodeType::Sun;

    float inf = std::numeric_limits<float>::infinity();
    mPosition = QVector3D(inf, inf, inf);
}

const QVector3D &Canavar::Engine::Sun::direction() const
{
    return mDirection;
}

void Canavar::Engine::Sun::setDirection(const QVector3D &newDirection)
{
    mDirection = newDirection;
}
