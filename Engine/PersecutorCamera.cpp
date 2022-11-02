#include "PersecutorCamera.h"

Canavar::Engine::PersecutorCamera::PersecutorCamera()
    : PerspectiveCamera()
    , mTarget(nullptr)
    , mDistance(10.0f)
    , mYaw(0.0f)
    , mPitch(0.0f)
{
    mType = Node::NodeType::PersecutorCamera;

    connect(this, &Camera::activeChanged, this, [=]() { update(0); });
}

void Canavar::Engine::PersecutorCamera::mouseDoubleClicked(QMouseEvent *) {}

void Canavar::Engine::PersecutorCamera::mousePressed(QMouseEvent *event)
{
    mMouse.mX = event->position().x();
    mMouse.mY = event->position().y();
    mMouse.mPressedButton = event->button();
}

void Canavar::Engine::PersecutorCamera::mouseReleased(QMouseEvent *event)
{
    if (mMouse.mPressedButton == event->button())
        mMouse.reset();
}

void Canavar::Engine::PersecutorCamera::mouseMoved(QMouseEvent *event)
{
    if (mMouse.mPressedButton == Qt::MiddleButton)
    {
        mMouse.mDx += mMouse.mX - event->position().x();
        mMouse.mDy += mMouse.mY - event->position().y();

        mMouse.mX = event->position().x();
        mMouse.mY = event->position().y();
    }
}
void Canavar::Engine::PersecutorCamera::wheelMoved(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0)
        mDistance += 0.5;

    if (event->angleDelta().y() > 0)
        mDistance -= 0.5;

    mDistance = qBound(0.1f, mDistance, 100.0f);
}

void Canavar::Engine::PersecutorCamera::keyPressed(QKeyEvent *) {}

void Canavar::Engine::PersecutorCamera::keyReleased(QKeyEvent *) {}

void Canavar::Engine::PersecutorCamera::update(float ifps)
{
    if (mTarget)
    {
        mYaw += mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDx * ifps;
        mPitch += mSpeed.mAngularMultiplier * mSpeed.mAngular * mMouse.mDy * ifps;

        auto newRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mYaw) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mPitch);
        auto newWorldPosition = mTarget->worldPosition() + mDistance * newRotation * QVector3D(0, 0, 1);

        setWorldPosition(newWorldPosition);
        setWorldRotation(newRotation);

        mMouse.mDx = 0.0f;
        mMouse.mDy = 0.0f;
    }
}

void Canavar::Engine::PersecutorCamera::reset()
{
    mDistance = 10.0f;
    mYaw = 0.0f;
    mPitch = 0.0f;
}

Canavar::Engine::Node *Canavar::Engine::PersecutorCamera::getTarget() const
{
    return mTarget;
}

void Canavar::Engine::PersecutorCamera::setTarget(Node *newTarget)
{
    if (mTarget)
        disconnect(mTarget, &QObject::destroyed, this, &PersecutorCamera::onTargetDestroyed);

    mTarget = newTarget;

    if (mTarget)
        connect(mTarget, &QObject::destroyed, this, &PersecutorCamera::onTargetDestroyed);

    reset();
}

void Canavar::Engine::PersecutorCamera::onTargetDestroyed()
{
    mTarget = nullptr;
}
