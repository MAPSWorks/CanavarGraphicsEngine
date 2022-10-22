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
    explicit PointLight(QObject *parent = nullptr);

    DECLARE_MEMBER(float, Constant)
    DECLARE_MEMBER(float, Linear)
    DECLARE_MEMBER(float, Quadratic)
};

} // namespace Engine
} // namespace Canavar

#endif // POINTLIGHT_H
