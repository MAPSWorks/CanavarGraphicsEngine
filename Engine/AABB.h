#ifndef AABB_H
#define AABB_H

#include "Common.h"

#include <QVector3D>

namespace Canavar {
namespace Engine {

class AABB
{
public:
    AABB();

    QVector3D getCenter() const { return (mMin + mMax) / 2.0f; }

private:
    DECLARE_MEMBER(QVector3D, Min);
    DECLARE_MEMBER(QVector3D, Max);
};

} // namespace Engine
} // namespace Canavar

#endif // AABB_H
