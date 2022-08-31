#include "FreeCamera.h"
#include "Helper.h"

#include <QTransform>
#include <QtMath>

FreeCamera::FreeCamera(QObject *parent)
    : PerspectiveCamera(parent)
    , mMode(Mode::RotateWhileMouseIsMoving)
    , mUpdateRotation(true)
    , mUpdatePosition(true)

{
    mNodeType = Node::NodeType::FreeCamera;

    connect(this, &Camera::activeChanged, this, [=](bool active) {
        if (!active)
            reset();
    });
}

FreeCamera::~FreeCamera() {}

void FreeCamera::onMouseDoubleClicked(QMouseEvent *)
{
    if (mAnimation.animating)
        return;
}

void FreeCamera::onWheelMoved(QWheelEvent *)
{
    if (mAnimation.animating)
        return;
}

void FreeCamera::onMousePressed(QMouseEvent *event)
{
    if (mAnimation.animating)
        return;

    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        mMouse.x = event->x();
        mMouse.y = event->y();
        mMouse.pressed = true;
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        if (event->button() == Qt::RightButton)
        {
            mMouse.grabbed = !mMouse.grabbed;
            mMouse.grabPosition = QPoint(event->localPos().x(), event->localPos().y());
            emit mouseGrabbed(mMouse.grabbed);
        }
        break;
    }
    }
}

void FreeCamera::onMouseReleased(QMouseEvent *)
{
    mMouse.pressed = false;
}

void FreeCamera::onMouseMoved(QMouseEvent *event)
{
    if (mAnimation.animating)
        return;

    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        if (mMouse.pressed)
        {
            mMouse.dx += mMouse.x - event->x();
            mMouse.dy += mMouse.y - event->y();

            mMouse.x = event->x();
            mMouse.y = event->y();
            mUpdateRotation = true;
        }
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        if (mMouse.grabbed)
        {
            if (mMouse.grabPosition.x() != event->pos().x() || mMouse.grabPosition.y() != event->pos().y())
            {
                mMouse.dx += mMouse.grabPosition.x() - event->x();
                mMouse.dy += mMouse.grabPosition.y() - event->y();
                mUpdateRotation = true;
            }

            emit setCursorPosition(mMouse.grabPosition);
        }
    }

    break;
    }
}

void FreeCamera::onKeyPressed(QKeyEvent *event)
{
    if (mAnimation.animating)
        return;

    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void FreeCamera::onKeyReleased(QKeyEvent *event)
{
    if (mAnimation.animating)
        return;

    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void FreeCamera::update(float ifps)
{
    if (mAnimation.animating)
    {
        float t = qMax(0.0f, qMin(1.0f, mTimeElapsed / mAnimation.duration));

        if (mAnimation.subject)
        {
            setWorldRotation(QQuaternion::slerp(mAnimation.startingRotation, mAnimation.subject->worldRotation(), t));
            setWorldPosition(mAnimation.startingPosition + (mAnimation.subject->worldPosition() - mAnimation.startingPosition) * t);
            setVerticalFov(mAnimation.startingVerticalFov + (mAnimation.subject->verticalFov() - mAnimation.startingVerticalFov) * t);
        } else
        {
            setWorldRotation(QQuaternion::slerp(mAnimation.startingRotation, mAnimation.finalRotation, t));
            setWorldPosition(mAnimation.startingPosition + (mAnimation.finalPosition - mAnimation.startingPosition) * t);
            setVerticalFov(mAnimation.startingVerticalFov + (mAnimation.finalVerticalFov - mAnimation.startingVerticalFov) * t);
        }

        if (qFuzzyCompare(t, 1.0f))
        {
            if (!mAnimation.saveFinalTransformation)
            {
                setWorldPosition(mPositionBeforeAnimation);
                setWorldRotation(mRotationBeforeAnimation);
                setVerticalFov(mVerticalFovBeforeAnimation);
            }

            mAnimation.animating = false;
            emit animationDone(mAnimation.activeCameraAfterAnimation);
        }

        mTimeElapsed += ifps;
        return;
    }

    // Rotation
    if (mUpdateRotation)
    {
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mSpeed.angularMultiplier * mSpeed.angular * mMouse.dx * ifps) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mSpeed.angularMultiplier * mSpeed.angular * mMouse.dy * ifps);

        mMouse.dx = 0.0f;
        mMouse.dy = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Control])
            mSpeed.movement = 1000.0f;
        else if (mPressedKeys[Qt::Key_Shift])
            mSpeed.movement = 100.0f;
        else
            mSpeed.movement = 5.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += mSpeed.movementMultiplier * mSpeed.movement * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mPressedKeys.empty())
        mUpdatePosition = false;
}

void FreeCamera::reset()
{
    mMouse.grabbed = false;
    emit mouseGrabbed(false);

    auto keys = mPressedKeys.keys();
    for (auto key : qAsConst(keys))
        mPressedKeys.insert(key, false);

    mMouse.pressed = false;
}

void FreeCamera::animate(const Animation &animation)
{
    PerspectiveCamera::mAnimation = animation;
    mAnimation.animating = true;
    mPositionBeforeAnimation = mPosition;
    mRotationBeforeAnimation = mRotation;
    mVerticalFovBeforeAnimation = mVerticalFov;
    mTimeElapsed = 0.0f;
}

void FreeCamera::drawGUI()
{
    if (!ImGui::CollapsingHeader("Parameters##FreeCamera"))
    {
        ImGui::SliderFloat("Movement Speed##FreeCamera", &mSpeed.movementMultiplier, 0, 100);
        ImGui::SliderFloat("Angular Speed##FreeCamera", &mSpeed.angularMultiplier, 0, 5);
        if (ImGui::SliderFloat("FOV##ProjectionParameters", &mVerticalFov, 1.0f, 120.0))
            setVerticalFov(mVerticalFov);
        ImGui::SliderFloat("Z-Near##ProjectionParameters", &mZNear, 0.1f, 100.0f);
        ImGui::SliderFloat("Z-Far##ProjectionParameters", &mZFar, 1000.0f, 1000000.0f);
    }

    if (!ImGui::CollapsingHeader("World Position##FreeCamera"))
    {
        QVector3D position = worldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::InputFloat("x##FreeCameraWorldPosition", &x, 0.01f, 10.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
        if (ImGui::InputFloat("y##FreeCameraWorldPosition", &y, 0.01f, 10.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
        if (ImGui::InputFloat("z##FreeCameraWorldPosition", &z, 0.01f, 10.0f, "%.3f"))
            setWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (!ImGui::CollapsingHeader("World Rotation##FreeCamera"))
    {
        QQuaternion rotation = worldRotation();
        float yaw, pitch, roll;

        Helper::getEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##FreeCamera", &yaw, 0.0f, 359.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##FreeCamera", &pitch, -89.999f, 89.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##FreeCamera", &roll, -179.999f, 179.999f, "%.3f"))
            setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }
}

bool FreeCamera::getMouseGrabbed() const
{
    return mMouse.grabbed;
}

const QMap<Qt::Key, QVector3D> FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};
