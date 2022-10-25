#include "Selectable.h"

Canavar::Engine::Selectable::Selectable()
{
    mAABB.setMin(QVector3D(-1, -1, -1));
    mAABB.setMax(QVector3D(1, 1, 1));
}
