#ifndef SPEED_H
#define SPEED_H

namespace Canavar {
namespace Engine {

class Speed
{
public:
    Speed();

    float mLinear;
    float mAngular;
    float mLinearMultiplier;
    float mAngularMultiplier;
};

} // namespace Engine
} // namespace Canavar

#endif // SPEED_H
