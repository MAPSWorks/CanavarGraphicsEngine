#include "Camera.h"

Canavar::Engine::Camera::Camera(QObject *parent)
    : Node(parent)
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mZNear(1.0f)
    , mZFar(10000.0f)
{
    mRenderable = false;
}

Canavar::Engine::Camera::~Camera()
{
    // TODO
}

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

int Canavar::Engine::Camera::width() const
{
    return mWidth;
}

void Canavar::Engine::Camera::setWidth(int newWidth)
{
    mWidth = newWidth;
}

int Canavar::Engine::Camera::height() const
{
    return mHeight;
}

void Canavar::Engine::Camera::setHeight(int newHeight)
{
    mHeight = newHeight;
}

float Canavar::Engine::Camera::zNear() const
{
    return mZNear;
}

void Canavar::Engine::Camera::setZNear(float newZNear)
{
    mZNear = newZNear;
}

float Canavar::Engine::Camera::zFar() const
{
    return mZFar;
}

void Canavar::Engine::Camera::setZFar(float newZFar)
{
    mZFar = newZFar;
}

void Canavar::Engine::Camera::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}
