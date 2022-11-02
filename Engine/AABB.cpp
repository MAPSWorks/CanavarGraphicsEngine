#include "AABB.h"

Canavar::Engine::AABB::AABB() {}

QMatrix4x4 Canavar::Engine::AABB::getTransformation() const
{
    QMatrix4x4 result;
    result.translate((mMin + mMax) / 2.0f);
    result.scale(mMax.x() - mMin.x(), mMax.y() - mMin.y(), mMax.z() - mMin.z());
    return result;
}
