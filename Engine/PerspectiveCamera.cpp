#include "PerspectiveCamera.h"
#include "Helper.h"

Canavar::Engine::PerspectiveCamera::PerspectiveCamera(QObject *parent)
    : Camera(parent)
{
    setVerticalFov(60);
}

Canavar::Engine::PerspectiveCamera::~PerspectiveCamera()
{
    // TODO
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::getProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, float(mWidth) / float(mHeight), mZNear, mZFar);
    return projection;
}

float Canavar::Engine::PerspectiveCamera::horizontalFov() const
{
    return mHorizontalFov;
}

void Canavar::Engine::PerspectiveCamera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;
    mVerticalFov = Helper::calculateVerticalFovForGivenHorizontalFov(mHorizontalFov, mWidth, mHeight);
}

float Canavar::Engine::PerspectiveCamera::verticalFov() const
{
    return mVerticalFov;
}

void Canavar::Engine::PerspectiveCamera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;
    mHorizontalFov = Helper::calculateHorizontalFovForGivenVerticalFov(mVerticalFov, mWidth, mHeight);
}
