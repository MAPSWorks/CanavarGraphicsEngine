#ifndef MOUSE_H
#define MOUSE_H

#include <QObject>

namespace Canavar {
namespace Engine {

class Mouse
{
public:
    Mouse();
    void reset();

    Qt::MouseButton mPressedButton;
    float mX;
    float mY;
    float mZ;
    float mDx;
    float mDy;
    float mDz;
};

} // namespace Engine
} // namespace Canavar

#endif // MOUSE_H
