#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(QObject *parent)
    : Camera{parent}
    , mZoom(15.0f)
    , mLeft(0)
    , mTop(0)
    , mUpdatePosition(false)

{
    Node::mPosition = QVector3D(0, 100000, 0);
    Node::mRotation = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90);
}

void OrthographicCamera::onMouseDoubleClicked(QMouseEvent *) {}

void OrthographicCamera::onMousePressed(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        mMouse.x = event->x();
        mMouse.y = event->y();
        mMouse.pressed = true;
    }
}

void OrthographicCamera::onMouseReleased(QMouseEvent *event)
{
    mMouse.pressed = false;
}

void OrthographicCamera::onMouseMoved(QMouseEvent *event)
{
    if (mMouse.pressed)
    {
        mMouse.dx += mMouse.x - event->x();
        mMouse.dy += mMouse.y - event->y();

        mMouse.x = event->x();
        mMouse.y = event->y();
        mUpdatePosition = true;
    }
}

void OrthographicCamera::onWheelMoved(QWheelEvent *event)
{
    if (event->angleDelta().y() < 0)
        mZoom = 1.1f * mZoom;
    else
        mZoom = mZoom / 1.1f;

    mZoom = qMax(1.0f, qMin(100.0f, mZoom));
}

void OrthographicCamera::onKeyPressed(QKeyEvent *) {}

void OrthographicCamera::onKeyReleased(QKeyEvent *) {}

void OrthographicCamera::update(float ifps)
{
    if (mUpdatePosition)
    {
        mLeft -= mZoom * mMouse.dx;
        mTop += mZoom * mMouse.dy;
        mMouse.dx = 0;
        mMouse.dy = 0;
        mUpdatePosition = false;
    }
}

void OrthographicCamera::drawGUI()
{
    if (!ImGui::CollapsingHeader("Camera##OrthographicCamera"))
    {
        ImGui::SliderFloat("Zoom##OrthographicCamera", &mZoom, 1, 100);
    }
}

QMatrix4x4 OrthographicCamera::projection()
{
    QMatrix4x4 projection;
    projection.ortho(-mZoom * mWidth / 2 - mLeft, //
                     mZoom * mWidth / 2 - mLeft,
                     -mZoom * mHeight / 2 - mTop,
                     mZoom * mHeight / 2 - mTop,
                     mZNear,
                     mZFar);
    return projection;
}

float OrthographicCamera::left() const
{
    return mLeft;
}

float OrthographicCamera::top() const
{
    return mTop;
}

float OrthographicCamera::zoom() const
{
    return mZoom;
}

void OrthographicCamera::setZoom(float newZoom)
{
    mZoom = newZoom;
}

void OrthographicCamera::setLeft(float newLeft)
{
    mLeft = newLeft;
}

void OrthographicCamera::setTop(float newTop)
{
    mTop = newTop;
}
