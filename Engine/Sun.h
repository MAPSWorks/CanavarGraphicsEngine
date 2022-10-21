#ifndef SUN_H
#define SUN_H

#include "Light.h"

namespace Canavar {
namespace Engine {

class Sun : public Light
{
private:
    explicit Sun(QObject *parent = nullptr);

public:
    static Sun *instance();

    const QVector3D &direction() const;
    void setDirection(const QVector3D &newDirection);

    bool enabled() const;
    void setEnabled(bool newEnabled);

protected:
    QVector3D mDirection;
    bool mEnabled;
};

} // namespace Engine
} // namespace Canavar

#endif // SUN_H
