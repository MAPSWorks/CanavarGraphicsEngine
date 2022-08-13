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

{
    mTransformation.setToIdentity();
    mProjection.setToIdentity();
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

void FreeCamera::onMousePressed(CustomMouseEvent event)
{
    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        mMousePreviousX = event.mouseEvent()->x();
        mMousePreviousY = event.mouseEvent()->y();
        mMousePressed = true;
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        break;
    }
    }
}

void FreeCamera::onMouseReleased(CustomMouseEvent)
{
    mMousePressed = false;
}

void FreeCamera::onMouseMoved(CustomMouseEvent event)
{
    switch (mMode)
    {
    case Mode::RotateWhileMouseIsPressing: {
        if (mMousePressed)
        {
            mMouseDeltaX += mMousePreviousX - event.mouseEvent()->x();
            mMouseDeltaY += mMousePreviousY - event.mouseEvent()->y();

            mMousePreviousX = event.mouseEvent()->x();
            mMousePreviousY = event.mouseEvent()->y();
            mUpdateRotation = true;
        }
        break;
    }
    case Mode::RotateWhileMouseIsMoving: {
        mMouseDeltaX += event.mouseGrabPosition().x() - event.mouseEvent()->x();
        mMouseDeltaY += event.mouseGrabPosition().y() - event.mouseEvent()->y();
        mUpdateRotation = true;
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
            mMovementSpeed = 50.0f;
        else if (mPressedKeys[Qt::Key_Control])
            mMovementSpeed = 1.0f;
        else
            mMovementSpeed = 5.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += mMovementSpeed * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mUpdatePosition | mUpdateRotation)
    {
        mTransformation.setToIdentity();
        mTransformation.rotate(mRotation.conjugated());
        mTransformation.translate(-mPosition);
    }

    if (mPressedKeys.empty())
    {
        mUpdatePosition = false;
    }
}

const QMap<Qt::Key, QVector3D> FreeCamera::KEY_BINDINGS = {
    {Qt::Key_W, QVector3D(0, 0, -1)},
    {Qt::Key_S, QVector3D(0, 0, 1)},
    {Qt::Key_A, QVector3D(-1, 0, 0)},
    {Qt::Key_D, QVector3D(1, 0, 0)},
    {Qt::Key_E, QVector3D(0, 1, 0)},
    {Qt::Key_Q, QVector3D(0, -1, 0)},
};
