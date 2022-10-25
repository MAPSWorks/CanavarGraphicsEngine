#ifndef SELECTABLE_H
#define SELECTABLE_H

#include "AABB.h"

namespace Canavar {
namespace Engine {

class Selectable
{
public:
    Selectable();

    DECLARE_MEMBER(AABB, AABB)
};

} // namespace Engine
} // namespace Canavar

#endif // SELECTABLE_H
