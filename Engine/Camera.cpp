#include "Camera.h"

Canavar::Engine::Camera::Camera(QObject *parent)
    : Node(parent)
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mZNear(1.0f)
    , mZFar(1000000.0f)
{}

QMatrix4x4 Canavar::Engine::Camera::getViewProjectionMatrix()
{
    return getProjectionMatrix() * getViewMatrix();
}

QMatrix4x4 Canavar::Engine::Camera::getViewMatrix()
{
    QMatrix4x4 viewMatrix;
    viewMatrix.rotate(mRotation.conjugated());
    viewMatrix.translate(-mPosition);

    if (mParent)
        return viewMatrix * mParent->worldTransformation().inverted();
    else
        return viewMatrix;
}

QMatrix4x4 Canavar::Engine::Camera::getRotationMatrix()
{
    auto rotation = getViewMatrix();
    rotation.setColumn(3, QVector4D(0, 0, 0, 1));
    return rotation;
}

QVector3D Canavar::Engine::Camera::getViewDirection()
{
    return getRotationMatrix().map(QVector4D(0, 0, -1, 0)).toVector3D();
}

bool Canavar::Engine::Camera::active() const
{
    return mActive;
}

void Canavar::Engine::Camera::setActive(bool newActive)
{
    if (mActive == newActive)
        return;
    mActive = newActive;
    emit activeChanged();
}

void Canavar::Engine::Camera::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}
