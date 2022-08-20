#include "Camera.h"

#include <QTransform>
#include <QtMath>

Camera::Camera(QObject *parent)
    : Node(parent)
    , mAspectRatio(1.0f)
    , mZNear(0.1f)
    , mZFar(10000.0f)
{
    setVerticalFov(60.0f);
}

float Camera::horizontalFov() const
{
    return mHorizontalFov;
}

void Camera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;

    float halfHorizontalFovRadian = 0.5f * qDegreesToRadians(mHorizontalFov);
    float verticalFovRadian = 2 * atan(tan(halfHorizontalFovRadian) * mAspectRatio);

    mVerticalFov = qRadiansToDegrees(verticalFovRadian);
}

float Camera::verticalFov() const
{
    return mVerticalFov;
}

void Camera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;

    float halfVerticalFovRadian = 0.5f * qDegreesToRadians(mVerticalFov);
    float horizontalFovRadian = 2 * atan(tan(halfVerticalFovRadian) / mAspectRatio);

    mHorizontalFov = qRadiansToDegrees(horizontalFovRadian);
}

float Camera::aspectRatio() const
{
    return mAspectRatio;
}

void Camera::setAspectRatio(float newAspectRatio)
{
    mAspectRatio = newAspectRatio;
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

QMatrix4x4 Camera::projection() const
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, mAspectRatio, mZNear, mZFar);
    return projection;
}

QVector3D Camera::getViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

QMatrix4x4 Camera::transformation() const
{
    QMatrix4x4 transformation;
    transformation.rotate(mRotation.conjugated());
    transformation.translate(-mPosition);

    return transformation;
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

bool Camera::active() const
{
    return mActive;
}

void Camera::setActive(bool newActive)
{
    mActive = newActive;

    emit activeChanged(mActive);
}
