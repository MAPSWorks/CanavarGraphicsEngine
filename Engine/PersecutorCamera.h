#ifndef PERSECUTORCAMERA_H
#define PERSECUTORCAMERA_H

#include "Mouse.h"
#include "PerspectiveCamera.h"
#include "Speed.h"

namespace Canavar {

namespace Engine {

class PersecutorCamera : public PerspectiveCamera
{
public:
    explicit PersecutorCamera(QObject *parent = nullptr);

    // Camera interface
public:
    void mouseDoubleClicked(QMouseEvent *) override;
    void mousePressed(QMouseEvent *) override;
    void mouseReleased(QMouseEvent *) override;
    void mouseMoved(QMouseEvent *) override;
    void wheelMoved(QWheelEvent *) override;
    void keyPressed(QKeyEvent *) override;
    void keyReleased(QKeyEvent *) override;
    void update(float) override;
    void reset() override;

    Node *target() const;
    void setTarget(Node *newTarget);

    void onTargetDestroyed();

private:
    Node *mTarget;
    Mouse mMouse;
    Speed mSpeed;

    DECLARE_MEMBER_CONST(float, Distance)
    DECLARE_MEMBER_CONST(float, Yaw)
    DECLARE_MEMBER_CONST(float, Pitch)
};

} // namespace Engine
} // namespace Canavar

#endif // PERSECUTORCAMERA_H
