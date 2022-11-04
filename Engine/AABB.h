#ifndef AABB_H
#define AABB_H

#include "Common.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QMatrix4x4>
#include <QVector3D>

namespace Canavar {
namespace Engine {

class AABB
{
public:
    AABB();

    QVector3D getCenter() const { return (mMin + mMax) / 2.0f; }

    QMatrix4x4 getTransformation() const;

    void toJson(QJsonObject &object);
    void fromJson(const QJsonObject &object);

private:
    DEFINE_MEMBER(QVector3D, Min);
    DEFINE_MEMBER(QVector3D, Max);
};

} // namespace Engine
} // namespace Canavar

#endif // AABB_H
