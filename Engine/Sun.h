#ifndef SUN_H
#define SUN_H

#include "Light.h"

namespace Canavar {
namespace Engine {

class Sun : public Light
{
protected:
    friend class NodeManager;
    explicit Sun(QObject *parent = nullptr);

public:
    const QVector3D &direction() const;
    void setDirection(const QVector3D &newDirection);

protected:
    QVector3D mDirection;
};

} // namespace Engine
} // namespace Canavar

#endif // SUN_H
