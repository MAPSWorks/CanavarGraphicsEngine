#ifndef PERSECUTORCAMERA_H
#define PERSECUTORCAMERA_H

#include "Mouse.h"
#include "PerspectiveCamera.h"
#include "Speed.h"

namespace Canavar {

namespace Engine {

class PersecutorCamera : public PerspectiveCamera
{
protected:
    friend class NodeManager;
    PersecutorCamera(const QString &uuid = QString());

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

    Node *getTarget() const;
    void setTarget(Node *newTarget);

private:
    void onTargetDestroyed();

private:
    Node *mTarget;

    Mouse mMouse;
    Speed mSpeed;

    DEFINE_MEMBER_CONST(float, Distance)
    DEFINE_MEMBER_CONST(float, Yaw)
    DEFINE_MEMBER_CONST(float, Pitch)
};

} // namespace Engine
} // namespace Canavar

#endif // PERSECUTORCAMERA_H
