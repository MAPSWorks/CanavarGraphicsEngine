#include "Camera.h"

Canavar::Engine::Camera::Camera()
    : Node()
    , mActive(false)
    , mWidth(1600)
    , mHeight(900)
    , mZNear(1.0f)
    , mZFar(1000000.0f)
{
    mSelectable = false;
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

QVector3D Canavar::Engine::Camera::getViewDirection()
{
    return mRotation * QVector3D(0, 0, -1);
}

float Canavar::Engine::Camera::calculateSkyYOffset(float horizonDistance)
{
    auto transform = QMatrix4x4();

    transform.setColumn(3, QVector4D(0, worldPosition().y(), 0, 1));
    auto ndc = (getProjectionMatrix() * transform) * QVector4D(0, 0, horizonDistance, 1);

    return ndc.y() / ndc.w();
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

void Canavar::Engine::Camera::toJson(QJsonObject &object)
{
    Node::toJson(object);

    object.insert("z_near", mZNear);
    object.insert("z_far", mZFar);
}

void Canavar::Engine::Camera::fromJson(const QJsonObject &object)
{
    Node::fromJson(object);

    setZNear(object["z_near"].toDouble());
    setZFar(object["z_far"].toDouble());
}

void Canavar::Engine::Camera::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}
