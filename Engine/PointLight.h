#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

namespace Canavar {
namespace Engine {

class NodeManager;

class PointLight : public Light
{
protected:
    friend class NodeManager;
    PointLight();

public:
    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

    DEFINE_MEMBER(float, Constant)
    DEFINE_MEMBER(float, Linear)
    DEFINE_MEMBER(float, Quadratic)
};

} // namespace Engine
} // namespace Canavar

#endif // POINTLIGHT_H
