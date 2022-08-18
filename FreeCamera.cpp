#include "FreeCamera.h"

#include <QTransform>
#include <QtMath>

FreeCamera::FreeCamera(QObject *parent)
    : Camera(parent)
    , mMovementSpeed(5.0f)
    , mAngularSpeed(25.0f)
    , mMousePressed(false)
    , mMousePreviousX(0.0f)
    , mMousePreviousY(0.0f)
    , mMouseDeltaX(0.0f)
    , mMouseDeltaY(0.0f)
    , mUpdateRotation(true)
    , mUpdatePosition(true)
    , mMode(Mode::RotateWhileMouseIsMoving)
    , mMouseGrabbed(false)

{
    mNodeType = Node::NodeType::FreeCamera;

    connect(this, &Camera::activeChanged, this, [=](bool active) {
        if (!active)
        {
            mMouseGrabbed = false;
            emit mouseGrabbed(false);
        }
    });
}

void FreeCamera::onKeyPressed(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
    mUpdatePosition = true;
}

void FreeCamera::onKeyReleased(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void FreeCamera::onMousePressed(QMouseEvent *event)
{
    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        mMousePreviousX = event->x();
        mMousePreviousY = event->y();
        mMousePressed = true;
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        if (event->button() == Qt::LeftButton)
        {
            mMouseGrabbed = !mMouseGrabbed;
            mMouseGrabPosition = QPoint(event->localPos().x(), event->localPos().y());
            emit mouseGrabbed(mMouseGrabbed);
        }
        break;
    }
    }
}

void FreeCamera::onMouseReleased(QMouseEvent *)
{
    mMousePressed = false;
}

void FreeCamera::onMouseMoved(QMouseEvent *event)
{
    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        if (mMousePressed)
        {
            mMouseDeltaX += mMousePreviousX - event->x();
            mMouseDeltaY += mMousePreviousY - event->y();

            mMousePreviousX = event->x();
            mMousePreviousY = event->y();
            mUpdateRotation = true;
        }
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        if (mMouseGrabbed)
        {
            if (mMouseGrabPosition.x() != event->pos().x() || mMouseGrabPosition.y() != event->pos().y())
            {
                mMouseDeltaX += mMouseGrabPosition.x() - event->x();
                mMouseDeltaY += mMouseGrabPosition.y() - event->y();
                mUpdateRotation = true;
            }

            emit setCursorPosition(mMouseGrabPosition);
        }
    }

    break;
    }
}

void FreeCamera::update(float ifps)
{
    // Rotation
    if (mUpdateRotation)
    {
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mAngularSpeed * mMouseDeltaX * ifps) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mAngularSpeed * mMouseDeltaY * ifps);

        mMouseDeltaY = 0.0f;
        mMouseDeltaX = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Shift])
            mMovementSpeed = 5000.0f;
        else if (mPressedKeys[Qt::Key_Control])
            mMovementSpeed = 100.0f;
        else
            mMovementSpeed = 5.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += mMovementSpeed * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mPressedKeys.empty())
        mUpdatePosition = false;
}

bool FreeCamera::getMouseGrabbed() const
{
    return mMouseGrabbed;
}

const QMap<Qt::Key, QVector3D> FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};
