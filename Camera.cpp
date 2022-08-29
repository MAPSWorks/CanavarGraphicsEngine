#include "Camera.h"

#include <QTransform>
#include <QtMath>

Camera::Camera(QObject *parent)
    : Node(parent)
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mZNear(1.0f)
    , mZFar(100000.0f)

{
    mRenderable = false;
}

Camera::~Camera() {}

bool Camera::active() const
{
    return mActive;
}

void Camera::setActive(bool newActive)
{
    mActive = newActive;

    emit activeChanged(mActive);
}

int Camera::width() const
{
    return mWidth;
}

void Camera::setWidth(int newWidth)
{
    mWidth = newWidth;
}

int Camera::height() const
{
    return mHeight;
}

void Camera::setHeight(int newHeight)
{
    mHeight = newHeight;
}

float Camera::zNear() const
{
    return mZNear;
}

void Camera::setZNear(float newZNear)
{
    mZNear = newZNear;
}

float Camera::zFar() const
{
    return mZFar;
}

void Camera::setZFar(float newZFar)
{
    mZFar = newZFar;
}

QMatrix4x4 Camera::transformation() const
{
    QMatrix4x4 t;
    t.rotate(mRotation.conjugated());
    t.translate(-mPosition);

    return t;
}

QMatrix4x4 Camera::worldTransformation() const
{
    Node *parent = dynamic_cast<Node *>(this->parent());

    if (parent)
    {
        QMatrix4x4 transformation;
        transformation.rotate(mRotation.conjugated());
        transformation.translate(-mPosition);

        QMatrix4x4 parentWorldTransformation = parent->worldTransformation();
        float x = parent->scale().x();
        float y = parent->scale().y();
        float z = parent->scale().z();

        parentWorldTransformation.scale(1 / x, 1 / y, 1 / z);

        return transformation * parentWorldTransformation.inverted();
    } else
    {
        return transformation();
    }
}

QMatrix4x4 Camera::getVP()
{
    return projection() * worldTransformation();
}

void Camera::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}
