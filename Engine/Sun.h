#ifndef SUN_H
#define SUN_H

#include "Light.h"

namespace Canavar {
namespace Engine {

class Sun : public Light
{
private:
    Sun();

protected:
    friend class NodeManager;
    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

public:
    static Sun *instance();

protected:
    DEFINE_MEMBER(bool, Enabled)
    DEFINE_MEMBER(QVector3D, Direction)
};

} // namespace Engine
} // namespace Canavar

#endif // SUN_H
