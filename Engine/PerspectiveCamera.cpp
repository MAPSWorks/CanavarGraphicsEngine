#include "PerspectiveCamera.h"
#include "Helper.h"

Canavar::Engine::PerspectiveCamera::PerspectiveCamera()
    : Camera()
{
    setVerticalFov(60);
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::getProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, float(mWidth) / float(mHeight), mZNear, mZFar);
    return projection;
}

const float &Canavar::Engine::PerspectiveCamera::getHorizontalFov() const
{
    return mHorizontalFov;
}

float &Canavar::Engine::PerspectiveCamera::getHorizontalFov_nonConst()
{
    return mHorizontalFov;
}

void Canavar::Engine::PerspectiveCamera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;
    mVerticalFov = Helper::calculateVerticalFovForGivenHorizontalFov(mHorizontalFov, mWidth, mHeight);
}

void Canavar::Engine::PerspectiveCamera::toJson(QJsonObject &object)
{
    Camera::toJson(object);

    object.insert("h_fov", mHorizontalFov);
    object.insert("v_fov", mVerticalFov);
}

void Canavar::Engine::PerspectiveCamera::fromJson(const QJsonObject &object)
{
    Camera::fromJson(object);

    mHorizontalFov = object["h_fov"].toDouble();
    mVerticalFov = object["v_fov"].toDouble();
}

const float &Canavar::Engine::PerspectiveCamera::getVerticalFov() const
{
    return mVerticalFov;
}

float &Canavar::Engine::PerspectiveCamera::getVerticalFov_nonConst()
{
    return mVerticalFov;
}

void Canavar::Engine::PerspectiveCamera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;
    mHorizontalFov = Helper::calculateHorizontalFovForGivenVerticalFov(mVerticalFov, mWidth, mHeight);
}
