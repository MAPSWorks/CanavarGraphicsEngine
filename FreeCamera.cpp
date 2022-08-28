#include "FreeCamera.h"
#include "Helper.h"

#include <QTransform>
#include <QtMath>

FreeCamera::FreeCamera(QObject *parent)
    : Camera(parent)
    , mMovementSpeed(5.0f)
    , mAngularSpeed(25.0f)
    , mMovementSpeedMultiplier(1.0f)
    , mAngularSpeedMultiplier(1.0f)
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

            auto keys = mPressedKeys.keys();
            for (auto key : qAsConst(keys))
                mPressedKeys.insert(key, false);
        }
    });
}

FreeCamera::~FreeCamera() {}

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
        if (event->button() == Qt::RightButton)
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
        mRotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouseDeltaX * ifps) * mRotation;
        mRotation = mRotation * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), mAngularSpeedMultiplier * mAngularSpeed * mMouseDeltaY * ifps);

        mMouseDeltaY = 0.0f;
        mMouseDeltaX = 0.0f;
        mUpdateRotation = false;
    }

    // Translation
    if (mUpdatePosition)
    {
        const QList<Qt::Key> keys = mPressedKeys.keys();

        if (mPressedKeys[Qt::Key_Control])
            mMovementSpeed = 1000.0f;
        else if (mPressedKeys[Qt::Key_Shift])
            mMovementSpeed = 100.0f;
        else
            mMovementSpeed = 5.0f;

        for (auto key : keys)
            if (mPressedKeys.value(key, false))
                mPosition += mMovementSpeedMultiplier * mMovementSpeed * ifps * mRotation.rotatedVector(KEY_BINDINGS.value(key, QVector3D(0, 0, 0)));
    }

    if (mPressedKeys.empty())
        mUpdatePosition = false;
}

void FreeCamera::drawGUI()
{
    if (!ImGui::CollapsingHeader("Speed##FreeCamera"))
    {
        ImGui::SliderFloat("Movement Speed##FreeCamera", &mMovementSpeedMultiplier, 0, 100);
        ImGui::SliderFloat("Angular Speed##FreeCamera", &mAngularSpeedMultiplier, 0, 5);
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
